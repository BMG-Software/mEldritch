#include "core.h"
#include "runtimeerror.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

RuntimeError::RuntimeError( const char* Message )
{
	const char* Prefix = "Runtime Error";
	SetMessage( "%s: %s", Prefix, Message );
}

void RuntimeError::SetMessage( const char* Format, ... )
{
	va_list ap;
	va_start( ap, Format );
	VSPRINTF( m_Message, RuntimeErrorMessageSize, Format, ap );
	va_end( ap );
}

void RuntimeError::Raise( const char* Message )
{
	throw RuntimeError( Message );
}

void RuntimeError::Raise( const char* Format, ... )
{
	va_list ap;
	va_start( ap, Format );
	int MsgLen = VSPRINTF_COUNT( Format, ap ) + 1;
	va_end( ap );
	char* Buffer = new char[ MsgLen ];
	va_start( ap, Format );
	VSPRINTF( Buffer, MsgLen, Format, ap );
	va_end( ap );
	RuntimeError TheError( Buffer );
	delete[] Buffer;
	throw TheError;
}
