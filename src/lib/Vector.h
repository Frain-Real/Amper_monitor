#ifndef LIB_VECTOR_H
#define LIB_VECTOR_H


#include <string.h>


template <typename T>
class Vector
{
public:
	Vector(int init=16, int add=0)
	{
		if (init < 1) init=1;
		if (add < 1) add=init;
		
		capacity=init;
		capacity_add=add;
		list=new T[capacity];
		length=0;
	}
	
	
	~Vector()
	{
		if (list) delete[] list;
	}
	
	
	int size()
	{
		return length;
	}
	
	
	bool append(T value)
	{
		if ( (list) && (ensureCapacity(length+1)) )
		{
			list[length++]=value;
			return true;
		} else
		{
			return false;
		}
	}
	
	
	T pop()
	{
		if ( (list) && (length > 0) )
		{
			return list[--length];
		} else
		{
			return 0;
		}
	}
	
	
	bool set(int idx, T value)
	{
		if ( (list) && (ensureCapacity(idx+1)) )
		{
			list[idx]=value;
			return true;
		} else
		{
			return false;
		}
	}
	
	
	T at(int idx)
	{
		if ( (list) && (idx>=0) && (idx < length) )
			return list[idx]; else
			return 0;
	}
	
	
protected:
	bool ensureCapacity(int len)
	{
		// Проверим - может хватит
		if (len <= capacity) return true;
		
		// Создаем новый массив
		len+=capacity_add;
		T *l2=new T[len];
		if (! l2) return false;
		
		// Копируем старый в него
		memcpy(l2, list, sizeof(T) * length);
		delete[] list;
		list=l2;
		capacity=len;
		
		return true;
	}
	
	
	T *list;
	int capacity, capacity_add, length;
};


#endif
