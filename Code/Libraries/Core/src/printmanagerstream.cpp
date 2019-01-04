#include "core.h"
#include "printmanagerstream.h"
#include "printmanager.h"
#include "hashedstring.h"

int PrintManagerStream::PrintF( const char* Str, ... ) const
{
	va_list Args;
	va_start( Args, Str );
	PrintManager::GetInstance()->VPrintf( HashedString::NullString, 0, Str, Args );
	va_end( Args );
	return PrintManager::GetInstance()->LastOutputLength();
}

int	PrintManagerStream::Read( int NumBytes, void* Buffer ) const { WARN; return 0; };
int	PrintManagerStream::Write( int NumBytes, const void* Buffer ) const { WARN; return 0; };
int	PrintManagerStream::SetPos( int Position ) const { WARN; return 0; };
int	PrintManagerStream::GetPos() const { WARN; return 0; };
int	PrintManagerStream::EOS() const { WARN; return 0; };
int	PrintManagerStream::Size() const { WARN; return 0; };
