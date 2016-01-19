#pragma once

enum class  TileType : int 
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
