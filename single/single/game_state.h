#pragma once

struct IGameState
{
	virtual void PoolEvent(sf::Event &) {}
	virtual void Draw() = 0;
	virtual void OnResize() = 0;
	virtual void Simulate() {}
};

struct TMapEditorState; //: IGameState
struct TMainMenuState;	//: IGameState
struct TMapParams;

struct TGameState : IGameState
{

	TGameState::TGameState();
	TGameState::~TGameState();
	TGameState::TGameState(const TGameState&) = delete;

	void TGameState::PoolEvent(sf::Event &) override;
	void TGameState::Simulate() override	{ mCurrentState->Simulate(); }
	void TGameState::Draw() override		{ mCurrentState->Draw(); }
	void TGameState::OnResize() override	{ mCurrentState->OnResize(); }

	void TGameState::GotoMapEditor_CreateMap(const TMapParams &);
	void TGameState::GotoMapEditor_LoadMap(const wchar_t* FileName);
	union {
		struct Prop_TMapEditorState: noncopyable {
			TMapEditorState* operator->(){ return BASEHACK(TGameState, MapEditorState, this)->mMapEditorState; }
		}MapEditorState;
		struct Prop_TMainMenuState: noncopyable {
			TMainMenuState* operator->() { return BASEHACK(TGameState, MapEditorState, this)->mMainMenuState; }
		}MainMenuState;
		struct Prop_IsClosed: noncopyable {
			operator bool() const { return CONSTBASEHACK(TGameState, MapEditorState, this)->mClosed; }		
		}IsClosed;
	};
	
private:
	TMainMenuState	*mMainMenuState = nullptr;
	TMapEditorState *mMapEditorState = nullptr;
	IGameState		*mCurrentState = nullptr;
	bool mClosed = false;
};