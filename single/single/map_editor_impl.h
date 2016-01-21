#pragma once


#include "map_editor.h"

TMapEditorState::TMapEditorState()
{
	if (!mTileMap.mTileTexture.loadFromFile("../../res/simple_tile_set.png"))
		throw std::exception("simple_tile_set.png not found");

	UpdateView();	

}

void TMapEditorState::CreateMap(const TMapParams& MapParams)
{
	mTileMap.Create(MapParams);
	mTileSprite.setTexture(mTileMap.mTileTexture);
	
	UpdateView();
	
}

void TMapEditorState::PoolEvent(sf::Event & Event)
{

}

void TMapEditorState::Simulate()
{
	static sf::Clock cl;
	sf::Time time = cl.getElapsedTime();
	if (time.asMilliseconds() > 50)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			mViewTileOffset.x--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			mViewTileOffset.y--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			mViewTileOffset.x++;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			mViewTileOffset.y++;
		}
		cl.restart();
	}
}
void TMapEditorState::Draw()
{
	Win.setView(mView);

	// размеры view в тайлах
	int txx = mViewTileSize.x;
	int tyy = mViewTileSize.y;

	// перебор тайлов влезающих в экран	
	for (int tx = 0; tx < txx; ++tx)
	{
		for (int ty = 0; ty < tyy; ++ty)
		{
			int xof = tx - mViewTileOffset.x;
			int yof = ty - mViewTileOffset.y;
			TileType t = mTileMap.TypeAt(uint16_t(xof), uint16_t(yof));
			if (t == TileType::Unknown)
				continue;
			sf::IntRect r( int(t)*TilePxSize, 0, TilePxSize, TilePxSize );
			mTileSprite.setTextureRect(r);
			sf::Vector2f pos_in_pixels(xof * TilePxSize, yof * TilePxSize);
			mTileSprite.setPosition(pos_in_pixels);
			Win.draw(mTileSprite);
		}
	}
}
void TMapEditorState::UpdateView()
{
	sf::Vector2u ws = Win.getSize();
	mViewTileSize.x = ws.x / TilePxSize;
	mViewTileSize.y = ws.y / TilePxSize;
	mView.reset( sf::FloatRect(0.0f, 0.0f, float(ws.x)/2, float(ws.y)/2) );
	
	
}
void TMapEditorState::OnResize() { UpdateView(); }