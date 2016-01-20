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
	TMenuItem& Font(sf::Font &Font)				{ text.setFont(Font);			return *this; }
	TMenuItem& CharSize(unsigned int sz)		{ text.setCharacterSize(sz);	return *this; }
	TMenuItem& Pos(float x, float y)			{ text.setPosition(x, y);		return *this; }

	sf::Keyboard::Key Key = sf::Keyboard::Unknown;
	void(*callback)(void*object) = nullptr;
	TMenuItem&		OnKey(sf::Keyboard::Key Key, void(*callback_ptr)(void* object))		{ this->Key = Key; callback = callback_ptr; return *this; }
	
	inline void TMenuItem::ProcessKey(sf::Keyboard::Key Key, void* object);
};

struct TMenu
{
	void *object_handler = nullptr;
	void ObjectHandler(void* pObjectHandler) { object_handler = pObjectHandler; }

	inline friend TMenu& operator+= (TMenu& m, TMenuItem& MenuItem) { m.items.push_back(MenuItem);	return m; }
	std::vector<TMenuItem> items;

	inline void TMenu::ProcessKey(sf::Keyboard::Key Key);
	inline void TMenu::Draw();
};



inline void TMenuItem::ProcessKey(sf::Keyboard::Key Key, void* object)
{
	if (Key == this->Key && callback)
		callback(object);
}

inline void TMenu::ProcessKey(sf::Keyboard::Key Key)
{
	for (auto mi : items)
		mi.ProcessKey(Key, object_handler);
}

inline void TMenu::Draw()
{
	for (auto mi : items)
		gpWin->draw(mi.text);
}


//----------------------------
//			TMainMenuState
//----------------------------
struct TMapParams;

struct TMainMenuState : IGameState
{
	TGameState *mState = nullptr;

	TMainMenuState::TMainMenuState();

	void TMainMenuState::PoolEvent(sf::Event &) override;
	void TMainMenuState::Simulate() override;
	void TMainMenuState::Draw() override;
	void TMainMenuState::OnResize() override;

	sf::Font mFont;
	enum { enCaption, enGenNewMap, enLoadMap,		enCount };
	TMenu mMenu;

	static void TMainMenuState::OnGenMap(void* This);
	static void TMainMenuState::OnLoadMap(void* This);

	
};



//----------------------------
//			TMainMenuState
//----------------------------


TMainMenuState::TMainMenuState()
{
	assert(mFont.loadFromFile("c:/windows/fonts/consola.ttf"));

	float x(100.0f), y(100.0f);
	mMenu.ObjectHandler(this);	// Yeeees, I know, it's must be template for type-safety, but... I hate template: bloat exe, increase compilation time and all IDE performance...
	
	mMenu += TMenuItem().Text(L"Бродилка")
		.Font(mFont).Pos(x, y).CharSize(30);
	y += 60;

	mMenu += TMenuItem().Text("1) Generate and edit map.")
		.Font(mFont).Pos(x, y).CharSize(20).OnKey(sf::Keyboard::Num1, &TMainMenuState::OnGenMap);		
	y += 30;

	mMenu += TMenuItem().Text("2) Load map...")
		.Font(mFont).Pos(x, y).CharSize(20).OnKey(sf::Keyboard::Num2, &TMainMenuState::OnLoadMap);		
	

}

void TMainMenuState::PoolEvent(sf::Event & Event)
{
	if (Event.type == sf::Event::KeyReleased)
		mMenu.ProcessKey(Event.key.code);
}

void TMainMenuState::Draw()
{
	mMenu.Draw();
}

void TMainMenuState::OnResize()
{
}

void TMainMenuState::Simulate()
{

}

INT_PTR __stdcall DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) /* more compact, each "case" through a single line, easier on the eyes */
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
		case IDOK:		
			PostQuitMessage(0);
			break;
		}
		break;

	case WM_CLOSE:
		PostQuitMessage(0);
		return TRUE;
	}
	return FALSE;
}


void TMainMenuState::OnGenMap(void *This)
{
	TMainMenuState &self = *((TMainMenuState*)This);
	
	// Спросим у юзера каким размеров карту ему хочется.
	HWND hDlg;
	MSG msg;
	BOOL ret;
	LRESULT lres;
	// Диалог -- IDD_NEW_MAP_DIALOG //
	hDlg = CreateDialogParam(gHinstance, MAKEINTRESOURCE(IDD_NEW_MAP_DIALOG), gpWin->getSystemHandle(), DialogProc, 0);
	assert(NULL != hDlg);
	
	HWND hWndComboBox = GetDlgItem(hDlg, IDC_COMBO_TILES);
	assert(NULL != hWndComboBox);

	for (int i = 0; i < int(TileType::Count); ++i )
	{
		const char* TileTypeName = TileType_Names[i];
		LRESULT res = SendMessageA(hWndComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)TileTypeName);
		assert(res == i);
		assert(res != CB_ERR);
		assert(res != CB_ERRSPACE);
	}
	lres = SendMessage(hWndComboBox, CB_SETCURSEL, /*index=*/0, 0);

	ShowWindow(hDlg, SW_SHOW);

	//UpdateWindow(hDlg);

	while ((ret = GetMessage(&msg, 0, 0, 0)) != 0)
	{
		if (ret == -1) {
			__debugbreak();
			break;
		}
		if (!IsDialogMessage(hDlg, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	// Считаем из диалога пользовательский ввод.
	TMapParams DialogField;
	assert( 0 != GetDlgItemText(hDlg, IDC_EDIT_FILENAME, const_cast<LPWSTR>(DialogField.FileName.c_str()), static_cast<int>(DialogField.FileName.size())) );
	BOOL ok;
	DialogField.w = GetDlgItemInt(hDlg, IDC_EDIT_WIDTH, &ok, /*signed =*/FALSE);
	assert(ok == TRUE);
	assert(DialogField.w > 0);
	assert(DialogField.w < 65535);
	DialogField.h = GetDlgItemInt(hDlg, IDC_EDIT_HEIGHT, &ok, /*signed =*/FALSE);
	assert(ok == TRUE);
	assert(DialogField.h > 0);
	assert(DialogField.h < 65535);
	DialogField.PrevalentTileType = TileTypeFromInt( (int)SendMessage(hWndComboBox, CB_GETCURSEL, 0, 0) );

	DestroyWindow(hDlg);

	self.mState->GotoMapEditor(DialogField);
}



void TMainMenuState::OnLoadMap(void *This)
{
	TMainMenuState &self = *((TMainMenuState*)This);
}
