#pragma once

struct IGameState
{
	virtual void PoolEvent(sf::Event &) {}
	virtual void Simulate() {}
	virtual void Draw() = 0;
	virtual void OnResize() = 0;
};

struct TMapEditorState; //: IGameState
struct TMainMenuState;	//: IGameState
struct TMapParams;

struct TGameState : IGameState
{

	TGameState::TGameState();
	TGameState::~TGameState();

	void TGameState::PoolEvent(sf::Event &) override;
	void TGameState::Simulate() override	{ mCurrentState->Simulate(); }
	void TGameState::Draw() override		{ mCurrentState->Draw(); }
	void TGameState::OnResize() override	{ mCurrentState->OnResize(); }
	bool TGameState::IsClosed() const		{ return mClosed; }

	void TGameState::GotoMapEditor_CreateMap(const TMapParams &);
	void TGameState::GotoMapEditor_LoadMap(const wchar_t* FileName);
//protected:
	TMainMenuState	*mMainMenuState = nullptr;
	TMapEditorState *mMapEditorState = nullptr;
	IGameState		*mCurrentState = nullptr;
	bool mClosed = false;
};