#include "parse.h"

#include <ctype.h>


int32_t parse_int(const char *s)
{
	int32_t v=0, sign=1;
	
	if ((*s)=='+') s++; else
	if ((*s)=='-')
	{
		s++;
		sign=-1;
	}
	
	while (isdigit((uint8_t)(*s)))
		v=v*10+(*(s++))-'0';
	
	return v*sign;
}


int64_t parse_int64(const char *s)
{
	int64_t v=0, sign=1;
	
	if ((*s)=='+') s++; else
	if ((*s)=='-')
	{
		s++;
		sign=-1;
	}
	
	while (isdigit((uint8_t)(*s)))
		v=v*10+(*(s++))-'0';
	
	return v*sign;
}


uint32_t parse_uint(const char *s)
{
	uint32_t v=0;
	
	while (isdigit((uint8_t)(*s)))
		v=v*10+(*(s++))-'0';
	
	return v;
}


uint64_t parse_uint64(const char *s)
{
	uint64_t v=0;
	
	while (isdigit((uint8_t)(*s)))
		v=v*10+(*(s++))-'0';
	
	return v;
}


float parse_float(const char *s)
{
	float v=0, sign=1.0;
	
	if ((*s)=='+') s++; else
	if ((*s)=='-')
	{
		s++;
		sign=-1;
	}
	
	while (isdigit((uint8_t)(*s)))
		v=v*10.0+((*(s++))-'0');
	
	if ( ((*s)=='.') || ((*s)==',') )
	{
		// Есть дробная часть
		float div=10.0;
		s++;
		while (isdigit((uint8_t)(*s)))
		{
			v=v+((float)((*(s++))-'0'))/div;
			div=div*10.0;
		}
	}
	
	return v*sign;
}


uint8_t parse_ip(const char *s, uint32_t *ip)
{
	uint8_t was_digit=0, ok=1;
	uint8_t n=0;
	uint8_t d[4]={0,0,0,0};
	
	for (; (*s) && (s[0]!='\n'); s++)
	{
		if ( (*s)=='.' )
		{
			if (!was_digit)
			{
				ok=0;
				break;
			}
			n++;
			if (n>=4) break;
			was_digit=0;
		} else
		if (isdigit((uint8_t)*s))
		{
			uint16_t x=d[n];
			x=x*10+((*s)-'0');
			if (x>255)
			{
				ok=0;
				break;
			}
			d[n]=x;
			was_digit=1;
		} else
		{
			// Неверный символ
			ok=0;
			break;
		}
	}
	
	if ( (ok) && (n==3) && (was_digit) )
	{
		// Получили IP-адрес
		(*ip)=(d[3] << 24) | (d[2] << 16) | (d[1] << 8) | d[0];
		return 1;
	}
	
	return 0;
}
