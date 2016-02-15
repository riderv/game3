#pragma once

#include "stdafx.h"
#include "main_menu.h"
#include "game_state.h"
#include "main_window.h"
#include "map_editor.h"

TMapEditorState::TMapEditorState(TGameState* BaseState)
	: mState(BaseState)
{
	mTileMap.mTilesetTexture = GameState.mTilesetTexture;
	UpdateView();	
	mCursorSprite.setTextureRect({ 0,0,TilePxSize,TilePxSize });
	mCursorSprite.setTexture(GameState.mTilesetTexture);
	mCursorSprite.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(Win)));
	mCursorSprite.setOrigin(TilePxSize / 2, TilePxSize / 2);
	// init menu
	{
		float x(0.0f), y(0.0f);
		mMenu.ObjectHandler(this);	// Yeeees, I know, it's must be template for type-safety, but... I hate template: bloat exe, increase compilation time and poor IDE performance...

		TMenuItem DefMenuItem;
		DefMenuItem.Font(GameState.mFont).CharSize(10);

		mMenu += TMenuItem(DefMenuItem)
			.Text("F1) Exit to main menu.")
			.Pos(x, y)
			.OnKey(sf::Keyboard::F1,[](void*s)
			{
				auto t = (TMapEditorState*)s;
				t->mTileMap = TTileMap();
				GameState.GotoMainMenu();
			}

		);
		y += 15;
		
		mMenu += TMenuItem(DefMenuItem)
			.Text("F5) Save")
			.Pos(x, y)
			.OnKey(sf::Keyboard::F5, &TMapEditorState::DoOnSave);
		y += 15;

		mMenu += TMenuItem(DefMenuItem)
			.Text("F6) Load")
			.Pos(x, y)
			.OnKey(sf::Keyboard::F6, &TMapEditorState::DoOnLoad);
		y += 15;

		mMenu += TMenuItem(DefMenuItem)
			.Text("~) Default tile brush")
			.Pos(x, y)
			.OnKey(sf::Keyboard::Tilde,
				[](void* s)
				{
					auto t = ((TMapEditorState*)s);
					t->mCursorSprite.setTextureRect({ t->mCurrentBrush*TilePxSize ,0,TilePxSize,TilePxSize });
					t->mCurrentBrush = t->mTileMap.mParam.DefaultTileType;
				}
		);
		y += 15;

		mMenu += TMenuItem(DefMenuItem)
			.Text("1) Ground brush")
			.Pos(x, y)
			.OnKey(sf::Keyboard::Num1,
				[](void* s)
				{
					auto t = ((TMapEditorState*)s);
					t->mCursorSprite.setTextureRect({TilePxSize*TTileType::Ground, 0, TilePxSize, TilePxSize});
					t->mCurrentBrush = TTileType::Ground;
				}
		);
		y += 15;

		mMenu += TMenuItem(DefMenuItem)
			.Text("2) Water brush")
			.Pos(x, y)
			.OnKey(sf::Keyboard::Num2,
				[](void* s)
				{
					auto t = ((TMapEditorState*)s);
					t->mCursorSprite.setTextureRect({ TilePxSize*TTileType::Water, 0, TilePxSize, TilePxSize });
					t->mCurrentBrush = TTileType::Water;
				}
		);
		y += 15;

		mMenu += TMenuItem(DefMenuItem)
			.Text("3) Stones brush")
			.Pos(x, y)
			.OnKey(sf::Keyboard::Num3,
				[](void* s)
				{
					auto t = ((TMapEditorState*)s);
					t->mCursorSprite.setTextureRect({ TilePxSize*TTileType::Stones, 0, TilePxSize, TilePxSize });
					t->mCurrentBrush = TTileType::Stones;
				}
		);
		y += 15;
	}
	mLastAction.setFont(GameState.mFont);
	mLastAction.setCharacterSize(20);
}

TMapEditorState::~TMapEditorState()
{
	// TODO надо ли выгружать текстуры, или SFML само?..
}

void TMapEditorState::CreateMap(const TMapParams& MapParams)
{
	mTileMap.Reset(MapParams);
	mTileSprite.setTexture(mTileMap.mTilesetTexture);
	mCurrentBrush = mTileMap.mParam.DefaultTileType;
	mCursorSprite.setTextureRect({ mCurrentBrush*TilePxSize,0,TilePxSize,TilePxSize });
	UpdateView();
	
}

void TMapEditorState::PoolEvent(sf::Event & Event)
{
	switch (Event.type)
	{
	case sf::Event::EventType::KeyReleased:
		mMenu.ProcessKey(Event.key.code);
		break;
	case sf::Event::EventType::MouseButtonPressed:
		OnMouseClick(Event.mouseButton.x, Event.mouseButton.y);
		break;
	}
}

void TMapEditorState::OnMouseClick(int x, int y)
{
	sf::Vector2f worldPos = Win.mapPixelToCoords({ x,y }, mView);
	int tx = int(worldPos.x / TilePxSize);
	int ty = int(worldPos.y / TilePxSize);
	tx -= mViewOffsetInTiles.x;
	ty -= mViewOffsetInTiles.y;
	TTileType tt = mTileMap.TypeAt(tx, ty);
	//MessageBoxA(0, TileName(tt), "tile under cursor", MB_OK);
	//MessageBoxA(0, TileName(mCurrentBrush), "tile under cursor", MB_OK);
	mTileMap.SafeSet(tx, ty, mCurrentBrush);

	sf::String msg = TileName(tt);
	msg += " -> ";
	msg += TileName(mCurrentBrush);
	mLastAction.setString( msg );
}

void TMapEditorState::Simulate()
{
	sf::Time time = mKeyDelayClock.getElapsedTime();
	if (time.asMilliseconds() > 15)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			if(mViewOffsetInTiles.x < (mViewSizeInTiles.x-1) )
				mViewOffsetInTiles.x++;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			if (mViewOffsetInTiles.y < (mViewSizeInTiles.y-1))
				mViewOffsetInTiles.y++;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			if (mViewOffsetInTiles.x < 0)
			{
				if (abs(mViewOffsetInTiles.x) < (mTileMap.mParam.w - 1))
					mViewOffsetInTiles.x--;
			}
			else
				mViewOffsetInTiles.x--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			if (mViewOffsetInTiles.y < 0)
			{
				if (abs(mViewOffsetInTiles.y) < (mTileMap.mParam.h - 1))
					mViewOffsetInTiles.y--;
			}
			else
				mViewOffsetInTiles.y--;

		}
		mKeyDelayClock.restart();
	}
}
void TMapEditorState::Draw()
{
	Win.setView(mView);

	// перебор тайлов влезающих в экран	
	for (int tx = 0; tx < mViewSizeInTiles.x; ++tx)
	{
		for (int ty = 0; ty < mViewSizeInTiles.y; ++ty)
		{
			int xof = tx - mViewOffsetInTiles.x;
			int yof = ty - mViewOffsetInTiles.y;
			
			TTileType Type = mTileMap.TypeAt(xof, yof);
			if (Type == TTileType::Unknown)
				continue;
			sf::IntRect r( int(Type)*TilePxSize, 0, TilePxSize, TilePxSize );
			mTileSprite.setTextureRect(r);
			sf::Vector2f pos_in_pixels(float(tx * TilePxSize), float(ty * TilePxSize));
			mTileSprite.setPosition(pos_in_pixels);
			Win.draw(mTileSprite);
		}
	}
	sf::Vector2u ws = Win.getSize();
	sf::View v({0,0, float(ws.x), float(ws.y)});
	Win.setView(v);
	mCursorSprite.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(Win)));
	Win.draw(mCursorSprite);
	mMenu.Draw(Win);
	Win.draw(mLastAction);
}
void TMapEditorState::UpdateView()
{
	sf::Vector2u ws = Win.getSize();
	
	// mView должен быть пропорционален экрану
	// но в два раза крупнее (впикселах)
	mView.reset( sf::FloatRect(0.0f, 0.0f, float(ws.x)/2, float(ws.y)/2) );
	mView.setViewport({ 1.0f/ws.x*50 ,0.0f,1.0f,1.0f });
	// записать сколько тайлов влезает во View и отрисовывать только влезающие
	auto vs = mView.getSize();
	mViewSizeInTiles.x = int(vs.x / TilePxSize);
	mViewSizeInTiles.y = int(vs.y / TilePxSize)+1;

	mLastAction.setPosition(10.0f, float(ws.y - mLastAction.getCharacterSize()) - 10.0f);
}
void TMapEditorState::OnResize() { UpdateView(); }

void TMapEditorState::DoOnSave(void *This_)
{
	TMapEditorState& This = *((TMapEditorState*)This_);
//	This.mTileMap.Save(This.mTileMap.Param.FileName);
	SQLite::TDB db;
	db.Open(This.mTileMap.mParam.FileName.c_str() );
	This.mTileMap.Save(db);

}

void TMapEditorState::DoOnLoad(void *This_)
{
	TMapEditorState& This = *((TMapEditorState*)This_);
//	This.mTileMap.Load(This.mTileMap.Param.FileName);
	SQLite::TDB db;
	db.Open(This.mTileMap.mParam.FileName.c_str());
	This.mTileMap.Load(db);	
}

void TMapEditorState::LoadMap(const wchar_t* FileName)
{
	try {
		SQLite::TDB db;
		db.Open(FileName);
		mTileMap.Load(db);
		mTileMap.mParam.FileName = FileName;
		mCurrentBrush = mTileMap.mParam.DefaultTileType;
		mTileSprite.setTexture(mTileMap.mTilesetTexture);
		mCursorSprite.setTextureRect({ mCurrentBrush*TilePxSize,0,TilePxSize,TilePxSize });

		UpdateView();
		return;
	}
	// TODO: сделать чтоли от одного предка?
	catch (const SQLite::exception& e)
	{
		MessageBoxA(0, e.msg.c_str(), "Error loading map.", MB_ICONERROR);
	}
	catch (const SQLite::wexception& e)
	{
		MessageBoxW(0, e.msg.c_str(), L"Error loading map.", MB_ICONERROR);
	}
	catch (const TException& e)
	{
		MessageBoxW(0, e.msg.c_str(), L"Error loading map.", MB_ICONERROR);
	}
	mTileMap = TTileMap();
	GameState.GotoMainMenu();
}

void TMapEditorState::ITileMapImpl::Set(int x, int y, TTileType val)
{
	TCoord2Int c(x, y);
	TMapEditorState* This = BASEHACK(TMapEditorState, miTileMap, this);
	This->mTileMap.Set(x,y, val);
}

TTileType TMapEditorState::ITileMapImpl::Get(int x, int y)
{
	TCoord2Int c(x, y);
	TMapEditorState* This = BASEHACK(TMapEditorState, miTileMap, this);
	TTileType ret = This->mTileMap.TypeAt(x, y);
	return ret;
}

//void TMapEditorState::DoDefBrush(void *This_)
//{
//	TMapEditorState* This = (TMapEditorState*)This_;
//}
//
//void TMapEditorState::DoGraundBrush(void *This_)
//{
//	TMapEditorState* This = (TMapEditorState*)This_;
//}
//
//void TMapEditorState::DoWaterBrush(void *This_)
//{
//	TMapEditorState* This = (TMapEditorState*)This_;
//}
//
//void TMapEditorState::DoStonesBrush(void *This_)
//{
//	TMapEditorState* This = (TMapEditorState*)This_;
//}
