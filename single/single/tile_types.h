#pragma once

#include "exception.h"

static const int TilePxSize = 16;
static const int CharPxHeigth = 32;

enum TTileType : ui16
{ Unknown,   Ground,   Water, Stones,  Count };

static const char *TileType_Names[] = 
{ "Unknown", "Graund", "Water", "Stones" };

constexpr const char* TileName(TTileType tt)
{
	return TileType_Names[int(tt)];
}

inline TTileType TileTypeFromInt(int val)
{
	if (val < int(TTileType::Unknown) || val >= int(TTileType::Count))
	{
		std::wstringstream ss;
		ss << L"Incorrect int value " << val << L" for TileType";
		throw TException(ss.str());
	}
	return TTileType(val);
}
