#include "StringBuffer.h"

#include <string.h>
#include <lib/xprintf.h>


StringBuffer::StringBuffer(int _alloc_size)
{
	alloc_size=_alloc_size;
	list=0;
	end=&list;
	constData=0;
}


StringBuffer::~StringBuffer()
{
	deleteConstData();
	clear();
}


int StringBuffer::length()
{
	// Вычисляем длину
	int len=0;
	struct L *l=list;
	while (l)
	{
		len+=l->len;
		l=l->next;
	}
	return len;
}


StringBuffer::operator const char*()
{
	if (! constData) constData=toNewString();
	return constData;
}


StringBuffer::operator String()
{
	return toString();
}


String StringBuffer::toString()
{
	return String(this);
}


int StringBuffer::toStringData(char *buf, int buflen)
{
	struct L *l=list;
	int len=0;
	while ( (l) && (buflen > 0) )
	{
		if (l->len < buflen)
		{
			memcpy(buf, l->str, l->len);
			buf+=l->len;
			buflen-=l->len;
			len+=l->len;
			l=l->next;
		} else
		{
			memcpy(buf, l->str, buflen);
			buf+=buflen;
			len+=buflen;
			break;
		}
	}
	return len;
}


int StringBuffer::toString(char *buf, int buflen)
{
	int len=toStringData(buf, buflen-1);
	if (buflen > 0) buf[len]=0;
	return len;
}


char* StringBuffer::toNewString()
{
	int len=length();
	char *str=new char[len+1];
	if (! str) return 0;
	toString(str, len+1);
	return str;
}


void StringBuffer::append(char c)
{
	// Удаляем временную строку
	deleteConstData();
	
	// Выделим место
	if (! alloc_if_need()) return;
	
	// Добавляем символ
	(*end)->str[(*end)->len++]=c;
	
	// Если буфер кончился - переключаемся на следующий
	if ((*end)->len >= alloc_size)
		end=&((*end)->next);
}


void StringBuffer::append(const char *str)
{
	// Удаляем временную строку
	deleteConstData();
	
	int len=strlen(str);
	
	while (len > 0)
	{
		// Выделим место
		if (! alloc_if_need()) return;
		
		// Копируем сколько влезет
		int l=alloc_size - (*end)->len;
		if (l > len) l=len;
		memcpy((*end)->str + (*end)->len, str, l);
		(*end)->len+=l;
		str+=l;
		len-=l;
		
		// Если буфер кончился - переключаемся на следующий
		if ((*end)->len >= alloc_size)
			end=&((*end)->next);
	}
}


void StringBuffer::appendf(const char *fmt, ...)
{
	// Удаляем временную строку
	deleteConstData();
	
	char *buf=new char[160];
	if (! buf) return;
	
	va_list args;
	
	va_start(args, fmt);
	xvsprintf(buf, fmt, args);
	va_end(args);
	
	append(buf);
	
	delete[] buf;
}


void StringBuffer::clear()
{
	// Удаляем временную строку
	deleteConstData();
	
	// Удаляем все фрагменты
	while (list)
	{
		delete[] list->str;
		struct L *del=list;
		list=list->next;
		delete del;
	}
	list=0;
	end=&list;
}


bool StringBuffer::alloc_if_need()
{
	// Проверим - может быть есть элемент
	if (*end) return true;
	
	// Создаем новый
	(*end)=new struct L;
	if (! (*end)) return false;
	(*end)->str=new char[alloc_size];
	if (! (*end)->str)
	{
		delete (*end);
		(*end)=0;
		return false;
	}
	(*end)->len=0;
	(*end)->next=0;
	
	return true;
}


void StringBuffer::deleteConstData()
{
	if (constData)
	{
		delete[] constData;
		constData=0;
	}
}
