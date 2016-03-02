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

union TCoord2UInt
{
	TCoord2UInt() :ui(0) {}
	TCoord2UInt(int x, int y) : x(ui16(x)), y(ui16(y)) { assert(x >= 0 && x <= Max<ui16>()); assert(y >= 0 && y <= Max<ui16>()); }
	explicit TCoord2UInt(ui32 val) : ui(val) {}

	ui32 ui;
	struct {
		ui16 x;
		ui16 y;
	};

	bool operator==(const TCoord2UInt& R) { return ui == R.ui;}
	operator uint32_t() const { return ui; }
};
	
typedef std::unordered_map< TCoord2UInt, TTileType, std::hash<uint32_t> > TTileMapInternal;

struct TTileMap
{
	
	TTileType	Get(int x, int y) const;
	void		Set(int x, int y, TTileType TileType);
	bool		SafeSet(int x, int y, TTileType TileType); // true if success

	void		Reset(const TMapParams& MapParams);

	void		Save(SQLite::TDB& db);
	void		Load(SQLite::TDB& db);

	int			CountOf( TTileType TileType ) const;

	TMapParams mParam;
	sf::Texture mTilesetTexture;

private:
	TTileMapInternal mMap;
};