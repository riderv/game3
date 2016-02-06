// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <Shlobj.h>
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "ole32.lib")


// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <cassert>

// TODO: reference additional headers your program requires here
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <Commctrl.h>

#include <sstream>
#include <fstream>  
#include <sstream>

#include <unordered_map>

#include <inttypes.h>

// ����� limits �� ��������
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include <limits>

#include <commdlg.h>

#include <wchar.h>

extern HINSTANCE gHinstance;

typedef uint64_t ui64;
typedef int64_t i64;
typedef uint32_t ui32;
typedef int32_t i32;
typedef uint16_t ui16;
typedef int16_t i16;
typedef uint8_t ui8;
typedef int8_t i8;

template <class T>
constexpr T Min()
{
	return std::numeric_limits<T>::min();
}

template <class T>
constexpr T Max()
{
	return std::numeric_limits<T>::max();
}

constexpr uint32_t ui32min()
{
	return Min<uint32_t>();
}
constexpr uint32_t ui32max()
{
	return Max<uint32_t>();
}

#define CHECK_RANGE(Val, Min, Max)\
	if(Val < Min || Val > Max){\
		std::stringstream s;\
		s << "CHECK_RANGE(val:" << Val << ", Min:" << Min << ", Max:" << Max << " failed.";\
		throw std::out_of_range(s.str()); };


#define SQLITE_API __declspec(dllimport)
#define SQLITE_STDCALL __stdcall

#include "sqlite3.h"

#define BASEHACK(Class, Member, Ptr) (   (Class*) ( ((char*)this) - (long long)&(((Class*)0)->Member) )   )
#define CONSTBASEHACK(Class, Member, Ptr) (   (const Class*) ( ((char*)this) - (long long)&(((Class*)0)->Member) )   )

struct noncopyable
{
	noncopyable& operator=(const noncopyable&) = delete;
	noncopyable(const noncopyable&) = delete;
};

struct nonassignable
{
	noncopyable& operator=(const noncopyable&) = delete;
};