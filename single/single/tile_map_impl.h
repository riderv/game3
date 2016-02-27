#pragma once


#include "tile_map.h"


void TTileMap::SafeSet(int x, int y, TTileType TileType)
{
	if (x < 0 || x >= mParam.w)
		return;
	if (y < 0 || y >= mParam.h)
		return;

	Set(x, y, TileType);
}
void TTileMap::Set(int x, int y, TTileType TileType)
{
	assert(x >= 0 && x < mParam.w);
	assert(y >= 0 && y < mParam.h);
	
	if (mParam.DefaultTileType == TileType)
	{
		auto i = mMap.find({x,y});
		if (i != mMap.end())
			mMap.erase(i);
	}
	else
	{
		mMap[{x, y}] = TileType;
	}
}

TTileType TTileMap::TypeAt(int x, int y) const
{
	if (x < 0 || y < 0)
		return TTileType::Unknown;
	if ( x >= mParam.w || y >= mParam.h)
		return TTileType::Unknown;

	TCoord2Int key;
	key.x = ui16(x);
	key.y = ui16(y);
	auto i = mMap.find(key);
	if( i == mMap.end() )
		return mParam.DefaultTileType;

	return i->second;
}

void TTileMap::Reset(const TMapParams& MapParams)
{
	mParam = MapParams;
	mMap.clear();
}

void TTileMap::Save(SQLite::TDB& db)
{
	auto Transaction = db.BeginTransaction();
	db.Exec("drop table if exists map");
	db.Exec("create table map("
			//"id INTEGER PRIMARY KEY,"
			"w INTEGER,"
			"h INTEGER,"
			"DefaultTileType INTEGER)"
	);
	{
		auto St = db.Prepare("INSERT INTO map VALUES(?,?,?)");
		St.Bind(1, mParam.w);
		St.Bind(2, mParam.h);
		St.Bind(3, mParam.DefaultTileType);
		if (St.Step() != SQLITE_DONE)
			db.Raise("INSERT INTO map VALUES(?,?,?); failed");

	}
	
	db.Exec("drop table if exists map_tiles;");
	db.Exec("create table map_tiles("
		"x INTEGER,"
		"y INTEGER,"
		"TileType INTEGER);"
		);
		{
			auto St = db.Prepare("INSERT INTO map_tiles VALUES(?, ?, ?)");
			for (auto p : mMap)
			{
				// WTF!? paramters 1-based??
				St.Bind(1, p.first.x);
				St.Bind(2, p.first.y);
				St.Bind(3, p.second); // tile type
				if (St.Step() != SQLITE_DONE)
					db.Raise("TTileMap::Save, on write mMap St.Step() failed.");
				St.Reset();
			}
		}
	Transaction.Commit();
	std::wstring fn = GetExePatch() + L"\\last_map_dir";
#pragma warning(disable:4996)
	FILE * F = _wfopen(fn.c_str(), L"wt, ccs=UTF-16LE");
#pragma warning(default:4996)
	if (!F)
		return;
	struct TFClose { FILE *f;  ~TFClose() { fclose(f); } } FClose = { F };
	auto key = L"last_map_dir=";
	fwrite(key, sizeof(key[0]), wcslen(key), F);
	std::wstring path = mParam.FileName;
	size_t bslash_pos = path.find_last_of(L'\\');
	if( bslash_pos != std::wstring::npos)
		path.erase(bslash_pos, path.end() - path.begin());
	fwrite(path.c_str(), sizeof(path[0]), path.size(), F);
	wchar_t endline[1] = { L'\n' };
	fwrite(endline, sizeof(endline[0]), 1, F);

}

void TTileMap::Load(SQLite::TDB& db)
{
	auto Transaction = db.BeginTransaction();
	TMapParams tmpParam = mParam;
	bool IsNull;
	// read map from database
	{
		auto Stmt = db.Prepare("select w,h,DefaultTileType from map");
		if (SQLITE_ROW != Stmt.Step())
		{
			db.Raise("TTileMap::Load, 'select w,h,DefaultTileType from map' failed. One row must present. Query return zero.");
		}
		Stmt.Get(0, tmpParam.w, IsNull); // WTF? 0-based unlike param-binding
		assert(IsNull == false);
		Stmt.Get(1, tmpParam.h, IsNull);
		assert(IsNull == false);
		ui16 Type;
		Stmt.Get(2, Type, IsNull);
		assert(IsNull == false);
		tmpParam.DefaultTileType = TTileType(Type);
		if (SQLITE_DONE != Stmt.Step())
		{
			db.Raise("TTileMap::Load, 'select w,h,DefaultTileType from map' failed. One row must present. Query return many.");
		}
	}
	
	// read (tile_type,x,y) to new map
	Index2TileType NewMap;

	{
		auto Stmt = db.Prepare("select x,y,TileType from map_tiles");
		ui16 Type;
		int ret;
		while (SQLITE_ROW == (ret = Stmt.Step()) )
		{
			TCoord2Int co;
			Stmt.Get(0, co.x, IsNull);
			assert(IsNull == false);
			
			Stmt.Get(1, co.y, IsNull);
			assert(IsNull == false);
			
			Stmt.Get(2, Type, IsNull);
			assert(IsNull == false);

			NewMap[co] = TTileType(Type);
			
		}
		assert(ret == SQLITE_DONE);
	}

	mParam = tmpParam;
	mMap.swap(NewMap);

	Transaction.Commit();
}

int TTileMap::CountOf( TTileType TileType ) const
{
	int Count = 0;
	if( mParam.DefaultTileType == TileType )
	{
		Count = mParam.w * mParam.h;
		Count -= IntFromSizet( mMap.size() );
		return Count;
	}
	
	for( auto tt : mMap )
	{
		if( tt.second == TileType )
		{
			Count++;
		}
	}
	return Count;
}

