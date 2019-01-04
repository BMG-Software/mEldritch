#include "core.h"
#include "customassert.h"
#include <stdio.h>
#include <stdarg.h>

#if BUILD_WINDOWS
#include <Windows.h>
#endif

#define ASSERTBUFFERSIZE 1024

namespace CustomAssert {

bool ShouldAssert( bool& Disable, const char* Assertion, ... )
{
#if BUILD_WINDOWS
	va_list Args;
	va_start( Args, Assertion );

	char AssertBuffer[ ASSERTBUFFERSIZE ];	// So we don't do any heap allocations in here
	const uint AssertionLength = VSPRINTF( AssertBuffer, ASSERTBUFFERSIZE, Assertion, Args );

	va_end( Args );

	strcpy_s( AssertBuffer + AssertionLength, ASSERTBUFFERSIZE - AssertionLength, "\nAbort to debug\nRetry to continue\nIgnore to continue and disable this assert" );

	const int Response = MessageBox( NULL, AssertBuffer, "Assertion failed", MB_ABORTRETRYIGNORE | MB_TASKMODAL | MB_SETFOREGROUND );

	if( Response == IDIGNORE )
	{
		Disable = true;
		return false;
	}
	else if( Response == IDABORT )
	{
		return true;
	}
	else
	{
		return false;
	}
#else
	// TODO PORT LATER: Add MessageBox type support for other platforms.
	Disable = true;
	return false;
#endif
}
}
