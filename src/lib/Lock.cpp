#include "Lock.h"


Lock::Lock(Semaphore _sem)
{
	sem=_sem;
	sem_lock(sem);
}


Lock::~Lock()
{
	sem_unlock(sem);
}
