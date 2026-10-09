#ifndef LIB_STRING_H
#define LIB_STRING_H


class StringBuffer;


class String
{
public:
	String();
	String(const char *s);
	String(const char *s, int len);
	String(StringBuffer *buf);
	String(const String &s);
	String& operator=(const String &s);
	~String();
	
	const char* s();
	void to(char *s, int maxlen=0);
	operator const char*();
	char operator[](int index);
	
	
protected:
	char *data;
	
	void decRef();
};


#endif
