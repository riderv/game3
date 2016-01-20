#pragma once

#include "game_state.h"

struct TMapEditorState : IGameState
{
	TGameState *mState = nullptr;
	TTileMap mTileMap;

		 TMapEditorState::TMapEditorState();
	void TMapEditorState::PoolEvent(sf::Event &) override;
	void TMapEditorState::Simulate() override;
	void TMapEditorState::Draw() override;
	void TMapEditorState::OnResize() override;

	void TMapEditorState::CreateMap(const TMapParams& MapParams);
	void TMapEditorState::UpdateView();
//protected:
	sf::View mView;
	sf::Vector2i mViewOffset = { 0, 0 };

	sf::View mMenuView;
	sf::Sprite mTileSprite;
	
};
