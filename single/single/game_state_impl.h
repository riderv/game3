#pragma once

#include "stdafx.h"
#include "game_state.h"
#include "main_menu.h"
#include "map_editor.h"

//----------------------------
//			TGameState
//----------------------------

TGameState::TGameState()
{
	LoadBaseFont();
	LoadBaseTileset();

	mCurrentState = mMainMenuState = new TMainMenuState(this);
}
TGameState::~TGameState()
{
	delete mMainMenuState;
	delete mMapEditorState;
	if (mFontBuf) {
		UnmapViewOfFile(mFontBuf);
		mFontBuf = 0;
	}
	if (mTilesetBuf) {
		UnmapViewOfFile(mTilesetBuf);
		mTilesetBuf = 0;
	}


}

void TGameState::PoolEvent(sf::Event &Event)
{
	if (Event.type == sf::Event::Closed)
		mClosed = true;						// do some action (maybe dialog)
	else
		mCurrentState->PoolEvent(Event);
}

void TGameState::GotoMapEditor_CreateMap(const TMapParams &MapParams)
{
	if (!mMapEditorState)
	{
		mMapEditorState = new TMapEditorState(this);
	}
	mCurrentState = mMapEditorState;
	mMapEditorState->CreateMap(MapParams);
	Win.setMouseCursorVisible(false);
	mMapEditorState->UpdateView();
	//union
	//{
	//	struct {
	//		uint16_t x, y;
	//	};
	//	uint32_t val;
	//} u;
	//for (int i = 0; i < 10; ++i) {
	//	u.x = rand() % MapParams.h;
	//	u.y = rand() % MapParams.w;
	//	mMapEditorState->mTileMap.mMap[u.val] = TileType::Water;
	//}
}

void TGameState::GotoMapEditor_LoadMap(const wchar_t* FileName)
{
	if (!mMapEditorState)
	{
		mMapEditorState = new TMapEditorState(this);
	}
	mCurrentState = mMapEditorState;
	mMapEditorState->LoadMap(FileName);
	Win.setMouseCursorVisible(false);
	mMapEditorState->UpdateView();
}

void TGameState::LoadBaseFont()
{
	if (mFontBuf == nullptr)
	{
		std::wstring ffn = GetExePatch() + L"/fonts/tahoma.ttf";
		auto res = MapFile(ffn);
		mFontBuf = res.first;
		if (false == GameState.Font.loadFromMemory(res.first, res.second))
		{
			std::wstring erm = L"sfml sf::Font::loadFromMemory failed";
			throw TException(erm);
		}
	}
}

void TGameState::LoadBaseTileset()
{
	if (mTilesetBuf == nullptr)
	{
		std::wstring fn = GetExePatch() + L"/res/simple_tile_set.png";
		auto res = MapFile(fn);
		mTilesetBuf = res.first;
		if (!mTilesetTexture.loadFromMemory(res.first, res.second)) //
			throw TException(L"simple_tile_set.png not found");
	}
}

void TGameState::GotoMainMenu()
{
	mCurrentState = mMainMenuState;
}