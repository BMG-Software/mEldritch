#ifndef MUTEX_H
#define MUTEX_H

#include <windows.h>

class Mutex
{
public:
	Mutex();
	~Mutex();

	void Create();
	void Wait();
	void Release();
	void Destroy();

private:
	HANDLE	m_hMutex;
};

#endif // MUTEX_H