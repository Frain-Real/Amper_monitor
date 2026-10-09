#ifndef LIB_JSON_H
#define LIB_JSON_H


#include <stdint.h>
#include <Vector.h>


class jsonValue;
class StringBuffer;


class jsonKeyValue
{
public:
	jsonKeyValue(const char *key, jsonValue *value, bool copy_key=true);
	~jsonKeyValue();
	
	jsonKeyValue* append(const char *key, jsonValue *value);
	
	
	const char *key;
	jsonValue *value;
	
	jsonKeyValue *next;
};


class jsonValue
{
public:
	enum
	{
		JNULL=0,
		BOOL,
		INT,
		FLOAT,
		STRING,
		ARRAY,
		OBJECT,
	};
	
	
	jsonValue();
	jsonValue(bool b);
	jsonValue(int i);
	jsonValue(int64_t i);
	jsonValue(float f);
	jsonValue(const char *s, bool copy_str=true);
	jsonValue(Vector<jsonValue*> *a);
	jsonValue(jsonKeyValue *o);
	
	~jsonValue();
	
	
	bool b();
	int i();
	int64_t l();
	float f();
	const char* s();
	
	int length();
	jsonValue* v(int index);
	jsonValue* v(const char *key);
	
	void toString(StringBuffer *sb);
	
	
	char type;
	union
	{
		bool    bValue;
		int64_t iValue;
		float   fValue;
		const char *sValue;
		Vector<jsonValue*> *aValue;
		jsonKeyValue *oValue;
	};
};


class jsonDeleteLater
{
public:
	jsonDeleteLater(jsonValue *v);
	~jsonDeleteLater();
	
protected:
	jsonValue *v;
};


jsonValue* jsonParse(const char *str);


#endif
