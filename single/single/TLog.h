#pragma once

#include "stdafx.h"

inline std::wstring LastErrStr();

inline std::string Utf16ToAnsi(const std::wstring& msg)
{
	int nUserNameLen = WideCharToMultiByte(
		CP_ACP, // ANSI Code Page
		0, // No special handling of unmapped chars
		msg.c_str(), // wide-character string to be converted
		static_cast<int>(msg.size()),
		NULL, 0, // No output buffer since we are calculating length
		NULL, NULL); // Unrepresented char replacement - Use Default 

	std::string s(nUserNameLen, '\0');
	WideCharToMultiByte(CP_ACP, // ANSI Code Page
		0, // No special handling of unmapped chars
		msg.c_str(), // wide-character string to be converted
		static_cast<int>(msg.size()),
		const_cast<char*>(s.c_str()),
		static_cast<int>(s.size()),
		NULL, NULL); // Unrepresented char replacement - Use Default
	return s;
}

inline std::wstring AnsiToUtf16(const std::string& msg)
{
	int Len = MultiByteToWideChar(
		CP_ACP,
		0,
		msg.c_str(),
		static_cast<int>(msg.size()),
		0, 0);
	std::wstring w(Len, L'\0');
	MultiByteToWideChar(
		CP_ACP,
		0,
		msg.c_str(),
		static_cast<int>(msg.size()),
		&w[0],
		static_cast<int>(w.size())
		);
	return w;
}

struct TLog
{
	HANDLE HFile;
	TLog() : HFile(NULL)
	{}
	void Init()
	{
		HFile = CreateFileA("log.txt",
			FILE_APPEND_DATA, 				//dwDesiredAccess
			FILE_SHARE_READ, 				// dwShareMode
			0, 								// ptr SECURITY_ATTRIBUTES
			OPEN_ALWAYS, 					// dwCreationDisposition
			FILE_FLAG_WRITE_THROUGH,        // dwFlagsAndAttributes
			NULL                            // hTemplateFile
			);
		if (INVALID_HANDLE_VALUE == HFile) {
			MessageBoxW(0, (L"EROR   CREATING   LOG FILE\n\n" + LastErrStr()).c_str(), L"error", MB_ICONERROR);
			return;
		}
		
		
		time_t rawtime;
		tm* timeinfo;
		static const int buf_len = 100;
		char buf[buf_len];
		time(&rawtime);
#pragma warning(push)
#pragma warning( disable: 4996 )
		timeinfo = localtime(&rawtime); 
#pragma warning( pop )

	
		strftime(buf, buf_len, "\r\n\r\n\r\nLog started: %G.%m.%d %H:%M:%S\r\n", timeinfo);
		

		DWORD bytes;
		WriteFile(HFile, buf, (DWORD)strlen(buf), &bytes, 0);

	}
	~TLog()
	{
		if (HFile) {
			CloseHandle(HFile);
			HFile = NULL;
		}
	}
	TLog& operator() (const std::string& msg)
	{
		std::wstring w = AnsiToUtf16(msg);
		operator()(w);
		return *this;
	}
	TLog& operator()(const std::wstring& msg)
	{
		ULONG bytes;
		if (0 == WriteFile(HFile, PVOID(msg.c_str()), (DWORD)msg.size(), &bytes, DWORD(NULL)))
		{
			MessageBoxW(0, (L"ERROR write log\n" + LastErrStr()).c_str(), L"error", MB_ICONERROR);
		}
		return *this;
	}

};
extern TLog Log;


inline std::wstring LastErrStr(DWORD errorMessageID)
{
	//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
	//Get the error message, if any.
	if (errorMessageID == 0)
		return std::wstring(); //No error message has been recorded

	wchar_t* messageBuffer = 0;
	size_t size = FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorMessageID,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), messageBuffer, 0, NULL);

	std::wstring message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;

}

inline std::wstring LastErrStr()
{
	//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	return LastErrStr(errorMessageID);
}

inline void ShowLastErr(DWORD err)
{
	std::wstring erm = LastErrStr(err);
	MessageBoxW(0, erm.c_str(), L"error", MB_ICONERROR);
}