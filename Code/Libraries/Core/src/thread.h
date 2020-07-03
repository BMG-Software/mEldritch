#ifndef THREAD_H
#define THREAD_H

#include <windows.h>

typedef DWORD ( WINAPI *ThreadFunc )( void* );

class Thread
{
public:
	Thread( ThreadFunc Function, void* Parameter );
	~Thread();

	void Wait();
	bool IsDone();

private:
	HANDLE	m_Thread;
};

#endif