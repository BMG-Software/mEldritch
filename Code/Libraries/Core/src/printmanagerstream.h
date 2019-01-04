#ifndef PRINTMANAGERSTREAM_H
#define PRINTMANAGERSTREAM_H

#include "printmanager.h"
#include "idatastream.h"

class PrintManagerStream : public IDataStream
{
public:
	virtual ~PrintManagerStream() {}

	virtual int	Read( int NumBytes, void* Buffer ) const;
	virtual int	Write( int NumBytes, const void* Buffer ) const;
	virtual int PrintF( const char* Str, ... ) const;
	virtual int	SetPos( int Position ) const;
	virtual int	GetPos() const;
	virtual int	EOS() const;
	virtual int	Size() const;
};

#endif // PRINTMANAGERSTREAM_H
