#pragma once

#include "game_state.h"

struct TMapEditorState : IGameState
{
		 TMapEditorState::TMapEditorState();
	void TMapEditorState::PoolEvent(sf::Event &) override;
	void TMapEditorState::Simulate() override;
	void TMapEditorState::Draw() override;
	void TMapEditorState::OnResize() override;

	void TMapEditorState::CreateMap(const TMapParams& MapParams);
	void TMapEditorState::UpdateView();
//protected:

	TGameState *mState = nullptr;
	TTileMap mTileMap;

	sf::View mView;
	sf::Vector2i mViewTileOffset = { 0, 0 };
	sf::Vector2i mViewTileSize = { 0, 0 };
	sf::Sprite mTileSprite;

};
