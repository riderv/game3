#pragma once


#include "tile_map.h"



TileType TTileMap::TypeAt(uint16_t x, uint16_t y) const
{
	if (x<0 || x > mParam.w || y <0 || y > mParam.h)
		return TileType::Unknown;

	TCoord2Int key;
	key.x = x;
	key.y = y;
	auto i = mMap.find(key.ui);
	if( i == mMap.end() )
		return mParam.PrevalentTileType;

	return i->second;
}

void TTileMap::Create(const TMapParams& MapParams)
{
	mParam = MapParams;
	mMap.clear();
}