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
	mCurrentState = mMainMenuState = new TMainMenuState;
	mMainMenuState->mState = this;
}
TGameState::~TGameState()
{
	delete mMainMenuState;
	delete mMapEditorState;
}

void TGameState::PoolEvent(sf::Event &Event)
{
	if (Event.type == sf::Event::Closed)
		mClosed = true;						// do some action (maybe dialog)
	else
		mCurrentState->PoolEvent(Event);
}

void TGameState::GotoMapEditor(const TMapParams &MapParams)
{
	if (!mMapEditorState)
	{
		mMapEditorState = new TMapEditorState();
		mMapEditorState->mState = this;
	}
	mCurrentState = mMapEditorState;
	mMapEditorState->UpdateView();
	mMapEditorState->CreateMap(MapParams);
	union
	{
		struct {
			uint16_t x, y;
		};
		uint32_t val;
	} u;
	for (int i = 0; i < 10; ++i) {
		u.x = rand() % MapParams.h;
		u.y = rand() % MapParams.w;
		mMapEditorState->mTileMap.mMap[u.val] = TileType::Water;
	}
}

