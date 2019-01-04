#ifndef RUNTIMEERROR_H
#define RUNTIMEERROR_H

#define RuntimeErrorMessageSize 256

class RuntimeError
{
private:
	RuntimeError( const char* Message );
	void SetMessage( const char* Format, ... );
	char m_Message[ RuntimeErrorMessageSize ];
public:
	const char* Message() { return m_Message; }

	static void Raise( const char* Message );
	static void Raise( const char* Format, ... );
};

#endif // RUNTIMEERROR_H
