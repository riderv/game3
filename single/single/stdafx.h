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

#ifdef _DEBUG
	#pragma comment(lib, "sfml-main-d.lib")
	#pragma comment(lib, "sfml-system-d.lib")
	#pragma comment(lib, "sfml-window-d.lib")
	#pragma comment(lib, "sfml-graphics-d.lib")
	#pragma comment(lib, "sqlite3.lib")
	#pragma comment(lib, "sfml-audio-d.lib")
#else
	#pragma comment(lib, "sfml-main.lib")
	#pragma comment(lib, "sfml-system.lib")
	#pragma comment(lib, "sfml-window.lib")
	#pragma comment(lib, "sfml-graphics.lib")
	#pragma comment(lib, "sqlite3.lib")
	#pragma comment(lib, "sfml-audio.lib")
#endif


// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <inttypes.h>
#include <cassert>

// crt
#define CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#include <vld.h> // memory leak detector

#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <Commctrl.h>

// STL
#include <memory>
#include <sstream>
#include <fstream>  
#include <sstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <functional>



// иначе limits не работает
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
		std::wstringstream s;\
		s << L"CHECK_RANGE(val:" << Val << L", Min:" << LMin << L", Max:" << Max << L" failed.";\
		throw TException(s); };


#define SQLITE_API __declspec(dllimport)
#define SQLITE_STDCALL __stdcall

#include "sqlite3.h"

#define BASEHACK(Class, Member, Ptr) (   (Class*) ( ((char*)this) - (long long)&(((Class*)0)->Member) )   )
#define CONSTBASEHACK(Class, Member, Ptr) (   (const Class*) ( ((char*)this) - (long long)&(((Class*)0)->Member) )   )

struct noncopyable
{
	noncopyable() {}
	noncopyable& operator=(const noncopyable&) = delete;
	noncopyable(const noncopyable&) = delete;
};

struct nonassignable
{
	noncopyable& operator=(const noncopyable&) = delete;
};


/*
template<class Cont>
class const_reverse_wrapper {
	const Cont& container;

public:
	const_reverse_wrapper(const Cont& cont) : container(cont) { }
	decltype(container.rbegin()) begin() const { return container.rbegin(); }
	decltype(container.rend()) end() const { return container.rend(); }
};

template<class Cont>
class reverse_wrapper {
	Cont& container;

public:
	reverse_wrapper(Cont& cont) : container(cont) { }
	decltype(container.rbegin()) begin() { return container.rbegin(); }
	decltype(container.rend()) end() { return container.rend(); }
};

template<class Cont>
const_reverse_wrapper<Cont> reverse(const Cont& cont) {
	return const_reverse_wrapper<Cont>(cont);
}

template<class Cont>
reverse_wrapper<Cont> reverse(Cont& cont) {
	return reverse_wrapper<Cont>(cont);
}
*/

