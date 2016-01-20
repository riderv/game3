#pragma once

#include "tile_types.h"


struct TMapParams
{
	std::wstring FileName;
	int w = 0; // width
	int h = 0; // heigth
	TileType PrevalentTileType = TileType::Unknown;

	TMapParams() { FileName.resize(1024); }
};

union TCoord2Int
{
	uint32_t ui;
	struct {
		uint16_t x;
		uint16_t y;
	};
};

struct TTileMap
{
	TileType TTileMap::TypeAt(uint16_t x, uint16_t y) const;
	void	 TTileMap::Create(const TMapParams& MapParams);

	TMapParams mParam;

	typedef std::unordered_map< uint32_t, TileType > Index2TileType;
	Index2TileType mMap;

	sf::Texture mTileTexture;
		
};