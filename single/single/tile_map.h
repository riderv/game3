#pragma once

#include "tile_types.h"
#include "sqlitedb.h"

struct TMapParams
{
	std::wstring FileName;
	int w = 0; // width
	int h = 0; // heigth
	TTileType DefaultTileType = TTileType::Unknown;

	TMapParams() {  }
};

union TCoord2Int
{
	TCoord2Int() :ui(0) {}
	TCoord2Int(int x, int y) : x(ui16(x)), y(ui16(y)) { assert(x >= 0 && x <= Max<ui16>()); assert(y >= 0 && y <= Max<ui16>()); }
	TCoord2Int(ui32 val) : ui(val) {}

	ui32 ui;
	struct {
		ui16 x;
		ui16 y;
	};

	bool operator==(const TCoord2Int& R) { return ui == R.ui;}
	operator uint32_t() const { return ui; }
};
	
typedef std::unordered_map< TCoord2Int, TTileType, std::hash<uint32_t> > Index2TileType;

struct TTileMap
{
	
	TTileType	TTileMap::TypeAt(int x, int y) const;
	void		TTileMap::Set(int x, int y, TTileType TileType);
	void		TTileMap::SafeSet(int x, int y, TTileType TileType);

	void	 TTileMap::Reset(const TMapParams& MapParams);

	void	 TTileMap::Save(SQLite::TDB& db);
	void	 TTileMap::Load(SQLite::TDB& db);

	int		 CountOf( TTileType TileType ) const;

	TMapParams mParam;
	sf::Texture mTilesetTexture;

private:
	Index2TileType mMap;		
};