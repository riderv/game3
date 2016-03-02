#pragma once
#include "TLog.h"
struct TException
{
	TException(const std::wstring& erm) : msg(erm) {}
	TException(const std::string& erm) : msg(AnsiToUtf16(erm))
	{
	}
	std::wstring msg;
};


inline int IntFromSizet( size_t val )
{
	if( val > INT_MAX ) {
		throw TException( "IntFromSizet: out of range" );
	}
	return (int)val;
}

inline void CHECK_TRUE( bool cond, const wchar_t* msg )
{
	if( false == cond )
		throw TException( std::wstring( L"Error: " ) + msg + L'\n' + LastErrStr() );
}

inline void CHECK_TRUE( bool cond, const std::wstring& msg )
{
	if( false == cond )
		throw TException( std::wstring( L"Error: " ) + msg + L'\n' + LastErrStr() );
}
