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
	
	// Объекты для эмуляции свойств. В union можно что нить полезное положить при случае.
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
		struct Prop_TilesetTexture : noncopyable {
			operator sf::Texture const& () const { return BASEHACK(TGameState, Font, this)->mTilesetTexture; }			
		}TilesetTexture;
	};
	//void SetFontBuf(void* buf) { assert(mFontBuf == nullptr); mFontBuf = buf; }
	//void Set_simple_tile_set_Buf(void *buf) { assert(mTilesetBuf == nullptr); mTilesetBuf = buf; }
	//void const* Get_simple_tile_set_Buf() const { return mTilesetBuf; }
	
	void TGameState::LoadBaseFont();
	void TGameState::LoadBaseTileset();
private:
	IGameState		*mCurrentState = nullptr;
	TMainMenuState	*mMainMenuState = nullptr;
	TMapEditorState *mMapEditorState = nullptr;
	TPlayState      *mPlayState = nullptr;
	bool mClosed = false;
	sf::Font mFont;
	void* mFontBuf = 0;
	sf::Texture mTilesetTexture;
	void *mTilesetBuf = 0;
	sf::Clock mClock;
public:
	ui64 DTmcs = 0;
};