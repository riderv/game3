#pragma once
#include "tile_types.h"

//----------------------------
//			TMainMenuState Helpers
//----------------------------

struct TMenuItem
{
	sf::Text text;
	TMenuItem& Text(const char* menu_text)		{ text.setString(menu_text);	return *this; }
	TMenuItem& Text(const wchar_t* menu_text)	{ text.setString(menu_text);	return *this; }
	TMenuItem& Font(const sf::Font &Font)				{ text.setFont(Font);			return *this; }
	TMenuItem& CharSize(unsigned int sz)		{ text.setCharacterSize(sz);	return *this; }
	TMenuItem& Pos(float x, float y)			{ text.setPosition(x, y);		return *this; }

	sf::Keyboard::Key Key = sf::Keyboard::Unknown;
	void(*callback)(void*object) = nullptr;
	inline TMenuItem& OnKey(sf::Keyboard::Key Key, void(*callback_ptr)(void* object));
	
	inline bool TMenuItem::ProcessKey(sf::Keyboard::Key Key, void* object);
};

struct TMenu
{
	void *object_handler = nullptr;
	void ObjectHandler(void* pObjectHandler) { object_handler = pObjectHandler; }

	inline friend TMenu& operator+= (TMenu& m, TMenuItem& MenuItem) { m.items.push_back(MenuItem);	return m; }
	std::vector<TMenuItem> items;

	inline void TMenu::ProcessKey(sf::Keyboard::Key Key);
	inline void TMenu::Draw(sf::RenderTarget& Target);
};





//----------------------------
//			TMainMenuState
//----------------------------
struct TMapParams;

struct TMainMenuState : IGameState
{
	TMainMenuState::TMainMenuState(TGameState* pGameState);
	TMainMenuState::TMainMenuState(const TMainMenuState&) = delete;

	void TMainMenuState::PoolEvent(sf::Event &) override;
	void TMainMenuState::Draw() override;
	void TMainMenuState::OnResize() override;

	
	enum { enCaption, enGenNewMap, enLoadMap,		enCount };
	TMenu mMenu;

	static void TMainMenuState::OnGenMap(void* This);
	static void TMainMenuState::OnLoadMap(void* This);
	
	union {
		struct Prop_Font: noncopyable {
			operator sf::Font&() const { return BASEHACK(TMainMenuState, Font, this)->mFont; }
		}Font;
	};
private:
	TGameState *mState = nullptr;
	sf::Font mFont;
};

