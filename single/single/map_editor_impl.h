#pragma once


#include "map_editor.h"

TMapEditorState::TMapEditorState()
{
	if (!mTileMap.mTileTexture.loadFromFile("../../res/simple_tile_set.png"))
		throw std::exception("simple_tile_set.png not found");
}

void TMapEditorState::CreateMap(const TMapParams& MapParams)
{
	mTileMap.Create(MapParams);
	mTileSprite.setTexture(mTileMap.mTileTexture);


}

void TMapEditorState::PoolEvent(sf::Event & Event)
{/*
	switch (Event.type)
	{
	case sf::Event::KeyPressed:
		if (Event.key.code == sf::Keyboard::Right)
		{
			mViewOffset.x++;
		}
		if (Event.key.code == sf::Keyboard::Left)
		{
			mViewOffset.x--;
		}
		if (Event.key.code == sf::Keyboard::Up)
		{
			mViewOffset.y--;
		}
		if (Event.key.code == sf::Keyboard::Down)
		{
			mViewOffset.y++;
		}
		break;
	default:
		break;
	}*/
}

void TMapEditorState::Simulate()
{
	static sf::Clock cl;
	sf::Time time = cl.getElapsedTime();
	if (time.asMilliseconds() > 50)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			mViewOffset.x--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			mViewOffset.y--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			mViewOffset.x++;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			mViewOffset.y++;
		}
		cl.restart();
	}
}
void TMapEditorState::Draw()
{
	//gpWin->setView(mView);

	// размеры view в тайлах
	int txx = int(gpWin->getView().getSize().x / 18);
	int tyy = int(gpWin->getView().getSize().y / 18);

		
	for (int tx = 0; tx < txx; ++tx)
	{
		for (int ty = 0; ty < tyy; ++ty)
		{
			TileType t = mTileMap.TypeAt(uint16_t(tx), uint16_t(ty));
			if (t == TileType::Unknown)
				continue;
			sf::IntRect r( int(t)*20+1, 1, 18, 18);
			mTileSprite.setTextureRect(r);
			sf::Vector2i pos_in_tiles(tx - mViewOffset.x, ty - mViewOffset.y);
			sf::Vector2f pos_in_pixels(pos_in_tiles.x*18.0f, pos_in_tiles.y*18.0f);
			mTileSprite.setPosition(pos_in_pixels);
			gpWin->draw(mTileSprite);
		}
	}
}

void TMapEditorState::OnResize()
{
	UpdateView();
}

void TMapEditorState::UpdateView()
{
	auto sz = gpWin->getSize();
	mView.setSize(float(sz.y), float(sz.y));
	mMenuView.setSize( float(sz.x - sz.y), float(sz.y));


}