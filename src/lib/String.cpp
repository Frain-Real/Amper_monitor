#include "String.h"

#include <stdint.h>
#include <string.h>
#include <StringBuffer.h>


#if defined(TARGET_ESP32)
	static inline bool isROM(const void *s)
	{
		uintptr_t A=(uintptr_t)s;
		return (A >= 0x3F400000) && (A < 0x3F800000);
	}
#elif defined(TARGET_STM32)
	static inline bool isROM(const void *s)
	{
		uintptr_t A=(uintptr_t)s;
		return (A >= 0x08000000) && (A < 0x08100000);
	}
#else
	#define isROM(s)	(false)
#endif


String::String()
{
	data=0;
}


String::String(const char *s)
{
	if (isROM(s))
	{
		data=(char*)s;
	} else
	{
		int l=strlen(s)+1;
		data=new char[4+l];
		*((int*)data)=1;
		memcpy(data+4, s, l);
	}
}


String::String(const char *s, int len)
{
	int l=len+1;
	data=new char[4+l];
	*((int*)data)=1;
	memcpy(data+4, s, l);
	data[4+len]=0;
}


String::String(StringBuffer *buf)
{
	int len=buf->length();
	int l=len+1;
	data=new char[4+l];
	*((int*)data)=1;
	buf->toStringData(data+4, len);
	data[4+len]=0;
}


String::String(const String &s)
{
	data=s.data;
	if (! isROM(data)) (*((int*)data))++;
}


String& String::operator=(const String &s)
{
	decRef();
	data=s.data;
	if (! isROM(data)) (*((int*)data))++;
	return *this;
}


String::~String()
{
	decRef();
}


const char* String::s()
{
	if (! data) return "";
	
	if (isROM(data))
		return (const char*)data; else
		return (const char*)(data+4);
}


void String::to(char *s, int maxlen)
{
	if (! data)
	{
		(*s)=0;
		return;
	}
	
	const char *src;
	if (isROM(data))
		src=data; else
		src=data+4;
	
	int len=strlen(src);
	if ( (maxlen > 0) && (len > maxlen-1) )
		len=maxlen-1;
	
	memcpy(s, src, len);
	s[len]=0;
}


String::operator const char*()
{
	if (! data) return "";
	
	if (isROM(data))
		return data; else
		return data+4;
}


char String::operator[](int index)
{
	const char *str=s();
	int len=strlen(str);
	if (index < len)
		return str[index]; else
		return 0;
}


void String::decRef()
{
	if (! data) return;
	
	if (! isROM(data))
	{
		(*((int*)data))--;
		if (*((int*)data) == 0)
		{
			delete[] data;
		}
	}
}
