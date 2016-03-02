#pragma once
#include "tile_types.h"
#include "sys.h"
//----------------------------
//			TMainMenuState Helpers
//----------------------------

struct TMenuItem
{
	sf::Text mText;
	sf::Keyboard::Key Key = sf::Keyboard::Unknown;
	typedef void( *callback_t )(void*);
	callback_t callback = nullptr;
	void *object = nullptr;

	TMenuItem& Text(const char* menu_text)		{ mText.setString(menu_text);	return *this; }
	TMenuItem& Text(const wchar_t* menu_text)	{ mText.setString(menu_text);	return *this; }
	TMenuItem& Font(const sf::Font &Font)		{ mText.setFont(Font);			return *this; }
	TMenuItem& CharSize(unsigned int sz)		{ mText.setCharacterSize(sz);	return *this; }
	TMenuItem& Pos(float x, float y)			{ mText.setPosition(x, y);		return *this; }

	template <class T>
	inline TMenuItem& OnKey(sf::Keyboard::Key Key, T*, void(*callback_ptr)(T*));
	
	inline bool ProcessKey(sf::Keyboard::Key Key);
};

struct TMenu
{
	inline friend TMenu& operator+= (TMenu& m, TMenuItem& MenuItem) { m.items.push_back(MenuItem);	return m; }
	std::vector<TMenuItem> items;

	inline void ProcessKey(sf::Keyboard::Key Key);
	inline void Draw(sf::RenderTarget& Target);
};


//----------------------------
//			TMainMenuState
//----------------------------
struct TMapParams;

struct TMainMenuState : IGameState, noncopyable
{
	inline TMainMenuState();
	~TMainMenuState() override;

	void PoolEvent(sf::Event &) override;
	void Draw() override;
	void OnResize() override;
	void Simulate() override {}

	static void OnGenMap(TMainMenuState *This);
	static void OnLoadMap(TMainMenuState *This);
	static void OnLoadAndPlay(TMainMenuState *This);

	enum { enCaption, enGenNewMap, enLoadMap,		enCount };
	TMenu mMenu;
};

