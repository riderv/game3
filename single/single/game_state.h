#pragma once

struct IGameState
{
	virtual void PoolEvent(sf::Event &) {}
	virtual void Simulate() {}
	virtual void Draw() = 0;
};


struct TMapEditorState : IGameState
{
	void TMapEditorState::PoolEvent(sf::Event &) override;
	void TMapEditorState::Simulate() override;
	void TMapEditorState::Draw() override;

	void TMapEditorState::GenerateMap();
protected:

};

struct TMainMenuState; //: IGameState

struct TGameState : IGameState
{

	TGameState::TGameState();
	TGameState::~TGameState();

	void TGameState::PoolEvent(sf::Event &) override;
	void TGameState::Simulate() override	{ mCurrentState->Simulate(); }
	void TGameState::Draw() override		{ mCurrentState->Draw(); }
	bool TGameState::IsClosed() const		{ return mClosed; }

	void TGameState::GotoNewMapEditor();
//protected:
	TMainMenuState	*mMainMenuState = nullptr;
	TMapEditorState *mMapEditorState = nullptr;
	IGameState		*mCurrentState = nullptr;
	bool mClosed = false;
};