#pragma once

#include "tile_types.h"
#include "sqlitedb.h"

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
	bool operator==(const TCoord2Int& R)
	{
		return ui == R.ui;
	}
	operator uint32_t() const { return ui; }
};

struct TTileMap
{
	
	TileType TTileMap::TypeAt(uint16_t x, uint16_t y) const;
	void	 TTileMap::Reset(const TMapParams& MapParams);

	void	 TTileMap::Save(SQLite::TDB& db);
	void	 TTileMap::Load(SQLite::TDB& db);

	TMapParams mParam;

	typedef std::unordered_map< TCoord2Int, TileType, std::hash<uint32_t> > Index2TileType;
	Index2TileType mMap;

	sf::Texture mTileTexture;
		
};