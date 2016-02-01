#pragma once

#include "stdafx.h"
#include "main_menu.h"
#include "game_state.h"
#include "main_window.h"
#include "map_editor.h"

TMapEditorState::TMapEditorState()
{
	//todo загрузку относительно ехе сделать
	if (!mTileMap.mTileTexture.loadFromFile("d:/_pro/game3/res/simple_tile_set.png"))
		throw std::exception("simple_tile_set.png not found");

	UpdateView();	

	// init menu
	{
		float x(0.0f), y(0.0f);
		mMenu.ObjectHandler(this);	// Yeeees, I know, it's must be template for type-safety, but... I hate template: bloat exe, increase compilation time and poor IDE performance...

		mMenu += TMenuItem()
			.Text("F5) Save")
			.Font(GameState.mMainMenuState->mFont)
			.Pos(x, y)
			.CharSize(20)
			.OnKey(sf::Keyboard::F5, &TMapEditorState::DoOnSave);
		y += 30;

		mMenu += TMenuItem()
			.Text("F6) Load")
			.Font(GameState.mMainMenuState->mFont)
			.Pos(x, y)
			.CharSize(20)
			.OnKey(sf::Keyboard::F6, &TMapEditorState::DoOnLoad);
	}

}

void TMapEditorState::CreateMap(const TMapParams& MapParams)
{
	mTileMap.Create(MapParams);
	mTileSprite.setTexture(mTileMap.mTileTexture);
	
	UpdateView();
	
}

void TMapEditorState::PoolEvent(sf::Event & Event)
{
	switch (Event.type)
	{
	case sf::Event::EventType::KeyReleased:
		mMenu.ProcessKey(Event.key.code);
		break;
	}
}

void TMapEditorState::Simulate()
{
	sf::Time time = mKeyDelayClock.getElapsedTime();
	if (time.asMilliseconds() > 50)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			mViewOffsetInTiles.x++;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			mViewOffsetInTiles.y++;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			mViewOffsetInTiles.x--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
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
			int xof = tx + mViewOffsetInTiles.x;
			int yof = ty + mViewOffsetInTiles.y;
			TileType Type = mTileMap.TypeAt(uint16_t(xof), uint16_t(yof));
			if (Type == TileType::Unknown)
				continue;
			sf::IntRect r( int(Type)*TilePxSize, 0, TilePxSize, TilePxSize );
			mTileSprite.setTextureRect(r);
			sf::Vector2f pos_in_pixels(float(xof * TilePxSize), float(yof * TilePxSize));
			mTileSprite.setPosition(pos_in_pixels);
			Win.draw(mTileSprite);
		}
	}

	//mMenu.Draw(Win);
}
void TMapEditorState::UpdateView()
{
	sf::Vector2u ws = Win.getSize();
	
	// mView должен быть пропорционален экрану
	// но в два раза крупнее (впикселах)
	mView.reset( sf::FloatRect(0.0f, 0.0f, float(ws.x)/2, float(ws.y)/2) );
	
	// записать сколько тайлов влезает во View и отрисовывать только влезающие
	mViewSizeInTiles.x = int(mView.getSize().x / TilePxSize);
	mViewSizeInTiles.y = int(mView.getSize().y / TilePxSize);
	
}
void TMapEditorState::OnResize() { UpdateView(); }

void TMapEditorState::DoOnSave(void *This)
{
	TMapEditorState& self = *((TMapEditorState*)This);
//	self.mTileMap.Save(self.mTileMap.mParam.FileName);
	SQLite::TDB db;
	db.Open(self.mTileMap.mParam.FileName.c_str() );
	self.mTileMap.Save(db);

}

void TMapEditorState::DoOnLoad(void *This)
{
	TMapEditorState& self = *((TMapEditorState*)This);
//	self.mTileMap.Load(self.mTileMap.mParam.FileName);
	SQLite::TDB db;
	db.Open(self.mTileMap.mParam.FileName.c_str());
	self.mTileMap.Load(db);
}

void TMapEditorState::LoadMap(const wchar_t* FileName)
{
	SQLite::TDB db;
	db.Open(FileName);
	mTileMap.Load(db);
}