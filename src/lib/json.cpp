#include "json.h"

#include <string.h>
#include <xprintf.h>
#include <ctype.h>
#include <parse.h>
#include <StringBuffer.h>


jsonKeyValue::jsonKeyValue(const char *_key, jsonValue *_value, bool copy_key)
{
	if (copy_key)
		key=xstrdup(_key); else
		key=_key;
	value=_value;
	next=0;
}


jsonKeyValue::~jsonKeyValue()
{
	if (key) xstrdel(key);
	if (value) delete value;
}


jsonKeyValue* jsonKeyValue::append(const char *_key, jsonValue *_value)
{
	jsonKeyValue **l=&next;
	while (*l) l=&((*l)->next);
	
	(*l)=new jsonKeyValue(_key, _value);
	
	return this;
}




jsonValue::jsonValue()
{
	type=JNULL;
}


jsonValue::jsonValue(bool b)
{
	type=BOOL;
	bValue=b;
}


jsonValue::jsonValue(int i)
{
	type=INT;
	iValue=i;
}


jsonValue::jsonValue(int64_t i)
{
	type=INT;
	iValue=i;
}


jsonValue::jsonValue(float f)
{
	type=FLOAT;
	fValue=f;
}


jsonValue::jsonValue(const char *s, bool copy_str)
{
	type=STRING;
	if (copy_str)
		sValue=xstrdup(s); else
		sValue=s;
}


jsonValue::jsonValue(Vector<jsonValue*> *a)
{
	type=ARRAY;
	aValue=a;
}


jsonValue::jsonValue(jsonKeyValue *o)
{
	type=OBJECT;
	oValue=o;
}


jsonValue::~jsonValue()
{
	switch (type)
	{
		case STRING:
			xstrdel(sValue);
			break;
		
		case ARRAY:
			for (int i=0; i<aValue->size(); i++)
				delete aValue->at(i);
			delete aValue;
			break;
		
		case OBJECT:
			while (oValue)
			{
				jsonKeyValue *next=oValue->next;
				delete oValue;
				oValue=next;
			}
			break;
	}
}


bool jsonValue::b()
{
	switch (type)
	{
		case BOOL:
			return bValue;
		
		case INT:
			return iValue != 0;
		
		case FLOAT:
			return fValue != 0;
		
		case STRING:
			return sValue[0] != 0;
		
		default:
			return false;
	}
}


int jsonValue::i()
{
	return l();
}


int64_t jsonValue::l()
{
	switch (type)
	{
		case BOOL:
			return bValue ? 1 : 0;
		
		case INT:
			return iValue;
		
		case FLOAT:
			if (fValue >= 0.0f)
				return (int64_t)(fValue + 0.5f); else
				return (int64_t)(fValue - 0.5f);
		
		case STRING:
			return parse_int64(sValue);
		
		default:
			return 0;
	}
}


float jsonValue::f()
{
	switch (type)
	{
		case BOOL:
			return bValue ? 1 : 0;
		
		case INT:
			return iValue;
		
		case FLOAT:
			return fValue;
		
		case STRING:
			return parse_float(sValue);
		
		default:
			return 0;
	}
}


const char* jsonValue::s()
{
	switch (type)
	{
		case STRING:
			return sValue;
		
		default:
			return "";
	}
}


int jsonValue::length()
{
	if (type == ARRAY)
	{
		return aValue->size();
	} else
	if (type == OBJECT)
	{
		int len=0;
		jsonKeyValue *kv=oValue;
		while (kv)
		{
			len++;
			kv=kv->next;
		}
		return len;
	} else
	{
		return 0;
	}
}


jsonValue* jsonValue::v(int index)
{
	if (type == ARRAY)
	{
		if ( (index >= 0) && (index < aValue->size()) )
			return aValue->at(index); else
			return 0;
	} else
	if (type == OBJECT)
	{
		if (index < 0) return 0;
		
		jsonKeyValue *kv=oValue;
		while ( (index--) && (kv) )
			kv=kv->next;
		
		if (! kv) return 0;
		return kv->value;
	} else
	{
		return 0;
	}
}


jsonValue* jsonValue::v(const char *key)
{
	if (type != OBJECT) return 0;
	
	jsonKeyValue *kv=oValue;
	while (kv)
	{
		if (! strcmp(kv->key, key)) return kv->value;
		kv=kv->next;
	}
	
	return 0;
}


static void jsonString(StringBuffer *sb, const char *str)
{
	sb->append('\"');
	for (const char *s=str; *s; s++)
	{
		switch (*s)
		{
			case '\n':
				sb->append("\\n");
				break;
			
			case '\t':
				sb->append("\\t");
				break;
			
			case '\"':
				sb->append("\\\"");
				break;
			
			default:
				sb->append(*s);
		}
	}
	sb->append('\"');
}


void jsonValue::toString(StringBuffer *sb)
{
	switch (type)
	{
		case JNULL:
			sb->append("null");
			break;
		
		case BOOL:
			sb->append(bValue ? "true" : "false");
			break;
		
		case INT:
			sb->appendf("%lld", iValue);
			break;
		
		case FLOAT:
			sb->appendf("%.4f", fValue);
			break;
		
		case STRING:
			jsonString(sb, sValue);
			break;
		
		case ARRAY:
			sb->append('[');
			for (int i=0; i<aValue->size(); i++)
			{
				aValue->at(i)->toString(sb);
				if (i < aValue->size()-1)
					sb->append(',');
			}
			sb->append(']');
			break;
		
		case OBJECT:
			sb->append('{');
			for (jsonKeyValue *kv=oValue; kv; kv=kv->next)
			{
				jsonString(sb, kv->key);
				sb->append(':');
				kv->value->toString(sb);
				if (kv->next) sb->append(',');
			}
			sb->append('}');
			break;
	}
}






static void skip_ws(const char **s)
{
	while (isspace(**s))
	{
		(*s)++;
	}
}


static const char* parseString(const char **str)
{
	// Проверяем на "
	if (**str != '\"') return 0;
	(*str)++;
	
	// Сохраняем позицию начала
	const char *s=*str;
	
	// Считаем длину и проверяем формат
	int len=0;
	while ( (**str) && (**str != '\"') )
	{
		if (**str == '\\')
		{
			// Escape
			(*str)++;
			switch (**str)
			{
				case 'n':
				case 't':
				case '\"':
				case '\\':
					break;
				
				default:
					return 0;
			}
		}
		
		len++;
		(*str)++;
	}
	if (**str != '\"') return 0;	// нет конца строки
	
	// Пропускаем закрывающую "
	(*str)++;
	
	// Создаем строку
	char *buf=new char[len+1], *ss=buf;
	while ( (*s) && (*s != '\"') )
	{
		if (*s == '\\')
		{
			// Escape
			s++;
			switch (*s)
			{
				case 'n':
					(*ss++)='\n';
					break;
				
				case 't':
					(*ss++)='\t';
					break;
				
				case '\"':
				case '\\':
					(*ss++)=(*s);
					break;
				
				default:
					return 0;
			}
		} else
		{
			(*ss++)=(*s);
		}
		
		s++;
	}
	(*ss++)=0;
	
	return buf;
}


static jsonValue* parseValue(const char **str)
{
	bool minus=false;
	
	switch ((*str)[0])
	{
		case 'f':
			// false
			if (strncmp(*str, "false", 5) != 0) return 0;
			(*str)+=5;
			return new jsonValue(false);
		
		
		case 't':
			// true
			if (strncmp(*str, "true", 4) != 0) return 0;
			(*str)+=4;
			return new jsonValue(true);
		
		
		case 'n':
			// null
			if (strncmp(*str, "null", 4) != 0) return 0;
			(*str)+=4;
			return new jsonValue();
		
		
		case '\"':
			// String
			{
				const char *s=parseString(str);
				if (! s) return 0;
				return new jsonValue(s, false);
			}
		
		
		case '[':
			// Array
			{
				Vector<jsonValue*> *items=new Vector<jsonValue*>();
				jsonValue *arr=new jsonValue(items);
				
				// Пропускаем '[' и пустые символы после
				(*str)++;
				skip_ws(str);
				
				// Проверим на пустой массив
				if (**str == ']')
				{
					(*str)++;
					return arr;
				}
				
				while (1)
				{
					// Получаем значение
					jsonValue *v=parseValue(str);
					if (! v)
					{
						// Ошибка
						break;
					}
					items->append(v);
					
					// Проверяем следующий элемент
					skip_ws(str);
					if (**str == ']')
					{
						// Конец массива
						(*str)++;
						return arr;
					} else
					if (**str == ',')
					{
						// Есть еще элементы
						(*str)++;
						skip_ws(str);
					} else
					{
						// Ошибка
						break;
					}
				}
				
				// Ошибка
				delete arr;
				return 0;
			}
		
		
		case '{':
			// Object
			{
				jsonKeyValue *items=0;
				jsonKeyValue **end=&items;
				
				// Пропускаем '{' и пустые символы после
				(*str)++;
				skip_ws(str);
				
				// Проверим на пустой объект
				if (**str == '}')
				{
					(*str)++;
					return new jsonValue(items);
				}
				
				while (1)
				{
					// Получаем ключ
					const char *key=parseString(str);
					if (! key) return 0;
					
					// Ожидаем :
					skip_ws(str);
					if (**str != ':')
					{
						delete[] key;
						break;
					}
					(*str)++;
					
					// Получаем значение
					skip_ws(str);
					jsonValue *value=parseValue(str);
					if (! value)
					{
						delete[] key;
						break;
					}
					
					// Добавляем к списку
					(*end)=new jsonKeyValue(key, value, false);
					end=&((*end)->next);
					
					// Проверяем следующий элемент
					skip_ws(str);
					if (**str == '}')
					{
						// Конец объекта
						(*str)++;
						return new jsonValue(items);
					} else
					if (**str == ',')
					{
						// Есть еще элементы
						(*str)++;
						skip_ws(str);
					} else
					{
						// Ошибка
						break;
					}
				}
				
				// Ошибка
				while (items)
				{
					jsonKeyValue *next=items->next;
					delete items;
					items=next;
				}
				return 0;
			}
		
		
		case '-':
			// Negative number
			minus=true;
			(*str)++;
			if (! isdigit(**str)) return 0;
			// break не нужен
		
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			// Number
			{
				int64_t iValue=0;
				do
				{
					iValue=(iValue*10) + ((**str)-'0');
					(*str)++;
				} while (isdigit(**str));
				
				if (**str != '.')
				{
					// Целое
					return new jsonValue(minus ? -iValue : iValue);
				} else
				{
					// Есть дробная часть
					float fValue=(float)iValue;
					float mul=0.1;
					
					(*str)++;	// пропускаем '.'
					while (isdigit(**str))
					{
						fValue+=(float)((**str)-'0') * mul;
						(*str)++;
					}
					
					return new jsonValue(minus ? -fValue : fValue);
				}
			}
		
		
		default:
			return 0;
	}
}



jsonDeleteLater::jsonDeleteLater(jsonValue *_v)
{
	v=_v;
}


jsonDeleteLater::~jsonDeleteLater()
{
	if (v) delete v;
}





jsonValue* jsonParse(const char *str)
{
	jsonValue *v=parseValue(&str);
	if (! v) return 0;
	if (! *str) return v;
	delete v;
	return 0;
}
