#ifndef LIB_LOCK_H
#define LIB_LOCK_H


#include <OS.h>


class Lock
{
public:
	Lock(Semaphore sem);
	~Lock();
	
protected:
	Semaphore sem;
};


#endif
