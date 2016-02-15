#pragma once

struct IGameState
{
	virtual IGameState::~IGameState() {}
	virtual void IGameState::PoolEvent(sf::Event &) {}
	virtual void IGameState::Draw() = 0;
	virtual void IGameState::OnResize() = 0;
	virtual void IGameState::Simulate() = 0;
};

struct TMapEditorState; //: IGameState
struct TMainMenuState;	//: IGameState
struct TPlayState;		//: IGameState
struct TMapParams;

struct TGameState : IGameState, noncopyable
{

	TGameState::TGameState();
	TGameState::~TGameState();

	void TGameState::PoolEvent(sf::Event &) override;
	void TGameState::Simulate() override	{ mCurrentState->Simulate(); }
	void TGameState::Draw() override		{ mCurrentState->Draw(); }
	void TGameState::OnResize() override	{ mCurrentState->OnResize(); }

	// state maschine transition
	//               state         substate
	void TGameState::GotoMapEditor_CreateMap(const TMapParams &);
	void TGameState::GotoMapEditor_LoadMap(const wchar_t* FileName);
	void TGameState::GotoMainMenu();
	void TGameState::GotoPlay_LoadMap(const wchar_t* FileName);
	

	void TGameState::LoadBaseFont();
	void TGameState::LoadBaseTileset();

	IGameState		*mCurrentState = nullptr;
	TMainMenuState	*mMainMenuState = nullptr;
	TMapEditorState *mMapEditorState = nullptr;
	TPlayState      *mPlayState = nullptr;
	bool mIsClosed = false;
	sf::Font mFont;
	void* mFontBuf = 0;
	sf::Texture mTilesetTexture;
	void *mTilesetBuf = 0;
	sf::Clock mClock;
	float DTs = 0;
};