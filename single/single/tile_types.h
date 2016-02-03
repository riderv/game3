#pragma once

static const int TilePxSize = 20;
// todo: заменить все int на int32
enum TTileType : ui16
{ Unknown,   Graund,   Water,  Count };

static const char *TileType_Names[] = 
{ "Unknown", "Graund", "Water" };

constexpr const char* TileName(TTileType tt)
{
	return TileType_Names[int(tt)];
}

inline TTileType TileTypeFromInt(int val)
{
	if (val < int(TTileType::Unknown) || val >= int(TTileType::Count))
	{
		std::stringstream ss;
		ss << "Incorrect int value " << val << " for TileType";
		throw std::out_of_range(ss.str());
	}
	return TTileType(val);
}
