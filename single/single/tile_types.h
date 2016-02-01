#pragma once

static const int TilePxSize = 20;
// todo: заменить все int на int32
enum TileType : ui16
{ Unknown,   Graund,   Water,  Count };

static const char *TileType_Names[] = 
{ "Unknown", "Graund", "Water" };

constexpr const char* TileName(TileType tt)
{
	return TileType_Names[int(tt)];
}

inline TileType TileTypeFromInt(int val)
{
	if (val < int(TileType::Unknown) || val >= int(TileType::Count))
	{
		std::stringstream ss;
		ss << "Incorrect int value " << val << " for TileType";
		throw std::out_of_range(ss.str());
	}
	return TileType(val);
}
