#include "core.h"
#include "hashedstring.h"
#include "simplestring.h"
#include "reversehash.h"

#pragma warning( disable: 4706 )	// assignment within conditional expression

const HashedString HashedString::NullString( "" );

HashedString::HashedString()
:	m_Hash( 0 )
{
}

HashedString::HashedString( uint32 Hash )
:	m_Hash( Hash )
{
}

HashedString::HashedString( const char* String )
:	m_Hash( Hash( String ) )
{
}

HashedString::HashedString( const SimpleString& String )
:	m_Hash( Hash( String.CStr() ) )
{
}

HashedString& HashedString::operator=( const char* String )
{
	m_Hash = Hash( String );
	return *this;
}

HashedString& HashedString::operator=( const SimpleString& String )
{
	m_Hash = Hash( String.CStr() );
	return *this;
}

bool HashedString::Equals( const HashedString& H ) const
{
	return ( m_Hash == H.m_Hash );
}

bool HashedString::IsNull() const
{
	return ( m_Hash == 0 );
}

uint32 HashedString::GetHash() const
{
	return m_Hash;
}

#if USE_HASH_FNV1A
// alternative hashing function: FNV-1a 32 bit
// only use if you're doing a full conversion!
uint32 HashedString::Hash( const char* const String )
{
	const uint32 offset_basis = 2166136261UL;
	const uint32 prime = 16777619UL;
	uint32 hash = offset_basis;
	for (const unsigned char* str = (const unsigned char*) String; *str; ++str) {
		hash = (hash ^ *str) * prime;
	}
	return hash;
}
#else
uint32 HashedString::Hash( const char* const String )
{
	if( !String )
	{
		return 0;
	}

	const char* CharIter = String;
	uint32 HashValue = 0;
	uint32 x;
	char c;
	while( c = *CharIter++ )
	{
		HashValue = ( HashValue << 4 ) + c;
		if( ( x = HashValue & 0xF0000000 ) != 0 )
		{
			HashValue ^= ( x >> 24 );
		}
		HashValue &= ~x;
	}

	if( ReverseHash::IsEnabled() )
	{
		ReverseHash::RegisterHash( HashedString( HashValue ), SimpleString( String ) );
	}

	return HashValue;
}
#endif
