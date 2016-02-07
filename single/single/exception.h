#pragma once
#include "TLog.h"
struct TException: std::exception
{
	TException(const std::wstring& erm) : msg(erm) {}
	TException(const std::string& erm) : msg(AnsiToUtf16(erm))
	{
	}
	std::wstring msg;
};
