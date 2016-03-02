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
	LoadSounds();

	mCurrentState = &mMainMenuState;
}
TGameState::~TGameState()
{
	if (mFontBuf)
		UnmapViewOfFile(mFontBuf);

	if (mTilesetBuf)
		UnmapViewOfFile(mTilesetBuf);

	for( auto s : mSounds )
		UnmapViewOfFile( s.mFileView );
}

void TGameState::PoolEvent(sf::Event &Event)
{
	if (Event.type == sf::Event::Closed)
		mIsClosed = true;						// do some action (maybe dialog)
	else
		mCurrentState->PoolEvent(Event);
}

void TGameState::GotoMapEditor_CreateMap(const TMapParams &MapParams)
{
	mCurrentState = &mMapEditorState;
	mMapEditorState.CreateMap(MapParams);
	Win.setMouseCursorVisible(false); // в редакторе свой курсор
	mMapEditorState.UpdateView();
}

void TGameState::GotoMapEditor_LoadMap(const wchar_t* FileName)
{
	mCurrentState = &mMapEditorState;
	mMapEditorState.LoadMap(FileName);
	Win.setMouseCursorVisible(false); // в редакторе свой курсор
	mMapEditorState.UpdateView();
}

void TGameState::GotoMainMenu()
{
	mCurrentState = &mMainMenuState;
	Win.setView( Win.getDefaultView() );
	Win.setMouseCursorVisible(true);
}

void TGameState::GotoPlay_LoadMap( const wchar_t* FileName )
{
	mCurrentState = &mPlayState;
	mPlayState.LoadAndPlay( FileName );
	Win.setMouseCursorVisible(true);
}

void TGameState::LoadBaseFont()
{
	if (mFontBuf == nullptr)
	{
		std::wstring ffn = GetExePatch() + L"/fonts/tahoma.ttf";
		auto res = MapFile(ffn);
		mFontBuf = res.first;
		if (false == GameState.mFont.loadFromMemory(res.first, res.second))
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
		std::wstring fn = GetExePatch() + L"/res/simple_tile_set_16x16a24.png";
		auto res = MapFile(fn);
		mTilesetBuf = res.first;
		if (!mTilesetTexture.loadFromMemory(res.first, res.second)) //
			throw TException(L"simple_tile_set_16x16a24.png not found");
	}
}

void TGameState::LoadSounds()
{
	std::wstring fn;
	mSounds.reserve( 11 );
	for( int i = 1; i < 12; ++i )
	{
		fn = GetExePatch() + L"/res/nyam" + IntToWStr(i) + L".ogg";
		auto res = MapFile( fn );
		mSounds.push_back( TGameState::TSound(res.first) );
		TGameState::TSound & s = *mSounds.rbegin();
		
		if( ! s.mSfmlSoundBuf.loadFromMemory(res.first, res.second) )
			throw TException( L"File not found: " + fn );
		s.mSound.setBuffer( s.mSfmlSoundBuf );
	}
}

