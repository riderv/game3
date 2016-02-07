#pragma once

#include "game_state.h"
struct ITileMap
{
	virtual ITileMap::~ITileMap() {}
	virtual void ITileMap::Set(int x, int y, TTileType val) = 0;
	virtual TTileType ITileMap::Get(int x, int y) = 0;
};

struct TMapEditorState : IGameState, noncopyable
{
		 TMapEditorState::TMapEditorState(TGameState* BaseState);
	void TMapEditorState::PoolEvent(sf::Event &) override;
	void TMapEditorState::Simulate() override;
	void TMapEditorState::Draw() override;
	void TMapEditorState::OnResize() override;

	void TMapEditorState::CreateMap(const TMapParams& MapParams);
	void TMapEditorState::UpdateView();

	// когда в редакторе карты жмём f5/f6
	static void TMapEditorState::DoOnSave(void *This_);
	static void TMapEditorState::DoOnLoad(void *This_);
	void TMapEditorState::OnMouseClick(int x, int y);

	// когда в главном меню выбираем LoadMap
	void TMapEditorState::LoadMap(const wchar_t* FileName);

	union {
		struct Prop_TileMap: noncopyable {
			ITileMap* operator->() { return &BASEHACK(TMapEditorState, TileMap, this)->miTileMap; }
		}TileMap;
	};
private:
	TGameState *mState = nullptr;
	TTileMap mTileMap;

	sf::View mView;
	sf::Vector2i mViewOffsetInTiles = { 0, 0 };
	sf::Vector2i mViewSizeInTiles = { 0, 0 };
	sf::Sprite mTileSprite;

	sf::Clock mKeyDelayClock;
	enum { enMenuSafe, enMenuLoad, enMenuCount };
	TMenu mMenu;
	sf::Sprite mCursorSprite;
	TTileType mCurrentBrush = mTileMap.mParam.DefaultTileType;

	struct ITileMapImpl: ITileMap
	{
		void      ITileMapImpl::Set(int x, int y, TTileType val) override;
		TTileType ITileMapImpl::Get(int x, int y) override;
	}miTileMap;

	
	sf::Text mLastAction;
};
