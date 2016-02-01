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

	// когда в редакторе карты жмём f5/f6
	static void TMapEditorState::DoOnSave(void *This);
	static void TMapEditorState::DoOnLoad(void *This);

	// когда в главном меню выбираем LoadMap
	void TMapEditorState::LoadMap(const wchar_t* FileName);
//protected:

	TGameState *mState = nullptr;
	TTileMap mTileMap;

	sf::View mView;
	sf::Vector2i mViewOffsetInTiles = { 0, 0 };
	sf::Vector2i mViewSizeInTiles = { 0, 0 };
	sf::Sprite mTileSprite;

	sf::Clock mKeyDelayClock;
	enum { enMenuSafe, enMenuLoad, enMenuCount };
	TMenu mMenu;
};
