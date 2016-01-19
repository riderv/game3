#pragma once

struct IGameState
{
	virtual void PoolEvent(sf::Event &) = 0;
	virtual void Simulate() = 0;
	virtual void Draw() = 0;
};

struct TMainMenuState : IGameState
{
	TMainMenuState::TMainMenuState();
	
	void TMainMenuState::PoolEvent(sf::Event &) override;
	void TMainMenuState::Simulate() override;
	void TMainMenuState::Draw() override;

	sf::Font mFont;
	enum { enCaption, enGenNewMap, enLoadMap, enLoadLastMap,
		   enCount };
	sf::Text mTxtMenus[enCount];
};

struct TMapEditorState : IGameState
{
	void TMapEditorState::PoolEvent(sf::Event &) override;
	void TMapEditorState::Simulate() override;
	void TMapEditorState::Draw() override;

	void TMapEditorState::GenerateMap();
protected:

};

struct TGameState : IGameState
{

	TGameState::TGameState();

	void TGameState::PoolEvent(sf::Event &) override;
	void TGameState::Simulate() override	{ mCurrentState->Simulate(); }
	void TGameState::Draw() override		{ mCurrentState->Draw(); }
	bool TGameState::IsClosed() const		{ return mClosed; }

	void TGameState::GotoNewMapEditor();
//protected:
	TMainMenuState mMainMenuState;
	TMapEditorState *mMapEditorState = nullptr;
	IGameState* mCurrentState = &mMainMenuState;
	bool mClosed = false;
};