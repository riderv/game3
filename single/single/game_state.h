#pragma once

struct IGameState // OOP pattern "State"
{
	virtual ~IGameState() {}
	virtual void PoolEvent(sf::Event &) {}
	virtual void Draw() = 0;
	virtual void OnResize() = 0;
	virtual void Simulate() = 0;
};

#include "main_menu.h"		// TMainMenuState  : IGameState
#include "map_editor.h"		// TMapEditorState : IGameState
#include "play_state.h"		// TPlayState	   : IGameState

struct TGameState : IGameState, noncopyable
{
	TGameState();
	~TGameState();

	void PoolEvent(sf::Event &) override;
	void Simulate() override	{ mCurrentState->Simulate(); }
	void Draw() override		{ mCurrentState->Draw(); }
	void OnResize() override	{ mCurrentState->OnResize(); }

	// state maschine transition
	//   state         substate
	void GotoMapEditor_CreateMap(const TMapParams &);
	void GotoMapEditor_LoadMap(const wchar_t* FileName);
	void GotoPlay_LoadMap(const wchar_t* FileName);
	void GotoMainMenu();
	

	void LoadBaseFont();
	void LoadBaseTileset();
	void LoadSounds();

//private:
	IGameState		*mCurrentState = nullptr;
	TMainMenuState	mMainMenuState;
	TMapEditorState mMapEditorState;
	TPlayState      mPlayState;
	bool mIsClosed = false;
	sf::Font mFont;
	void* mFontBuf = 0;
	sf::Texture mTilesetTexture;
	void *mTilesetBuf = 0;
	sf::Clock mClock;
	float DTs = 0;

	struct TSound {
		void* mFileView = 0;
		sf::SoundBuffer mSfmlSoundBuf;
		sf::Sound mSound;
		TSound( void* buf )
			: mFileView( buf ) {}
	};
	std::vector<TSound> mSounds;
};