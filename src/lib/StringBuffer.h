#ifndef LIB_STRINGBUFFER_H
#define LIB_STRINGBUFFER_H


#include <String.h>


class StringBuffer
{
public:
	StringBuffer(int alloc_size=64);
	~StringBuffer();
	
	int length();
	operator const char*();
	operator String();
	String toString();
	int toStringData(char *buf, int buflen);
	int toString(char *buf, int buflen);
	char* toNewString();
	
	void append(char c);
	void append(const char *str);
	void appendf(const char *fmt, ...);
	
	void clear();
	
	
private:
	bool alloc_if_need();
	void deleteConstData();

	
	
	int alloc_size;
	
	struct L
	{
		char *str;
		int len;
		
		struct L *next;
	} *list, **end;
	
	const char *constData;
};


#endif
