#pragma once

struct IGameState
{
	~IGameState() {}
	virtual void IGameState::PoolEvent(sf::Event &) {}
	virtual void IGameState::Draw() = 0;
	virtual void IGameState::OnResize() = 0;
	virtual void IGameState::Simulate() {}
};

struct TMapEditorState; //: IGameState
struct TMainMenuState;	//: IGameState
struct TMapParams;

struct TGameState : IGameState, noncopyable
{

	TGameState::TGameState();
	TGameState::~TGameState();

	void TGameState::PoolEvent(sf::Event &) override;
	void TGameState::Simulate() override	{ mCurrentState->Simulate(); }
	void TGameState::Draw() override		{ mCurrentState->Draw(); }
	void TGameState::OnResize() override	{ mCurrentState->OnResize(); }

	void TGameState::GotoMapEditor_CreateMap(const TMapParams &);
	void TGameState::GotoMapEditor_LoadMap(const wchar_t* FileName);
	void TGameState::GotoMainMenu();

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

		struct Prop_Font : noncopyable {
			operator sf::Font const&() const { return BASEHACK(TGameState, Font, this)->mFont; }
			bool loadFromMemory(void* buf, size_t size) { return BASEHACK(TGameState, Font, this)->mFont.loadFromMemory(buf, size); }
		}Font;		
	};
	//void SetFontBuf(void* buf) { assert(mFontBuf == nullptr); mFontBuf = buf; }
	//void Set_simple_tile_set_Buf(void *buf) { assert(mTilesetBuf == nullptr); mTilesetBuf = buf; }
	//void const* Get_simple_tile_set_Buf() const { return mTilesetBuf; }
	
	void TGameState::LoadBaseFont();
	void TGameState::LoadBaseTileset();
	const sf::Texture& GetTilesetTexture() const { return mTilesetTexture; }
private:
	TMainMenuState	*mMainMenuState = nullptr;
	TMapEditorState *mMapEditorState = nullptr;
	IGameState		*mCurrentState = nullptr;
	bool mClosed = false;
	sf::Font mFont;
	void* mFontBuf = 0;
	sf::Texture mTilesetTexture;
	void *mTilesetBuf = 0;
};