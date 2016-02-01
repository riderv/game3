#pragma once

// все эти инклуды лишь чтобы удовлетворить подсказчик кода
#include "stdafx.h"
#include "TLog.h"
#include "tile_types.h"
#include "main_menu.h"
#include "game_state.h"
#include "main_window.h"
#include "resource.h"
#include "map_editor.h"
//----------------------------
//			TMenuItem
//----------------------------

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

inline void TMenu::Draw(sf::RenderTarget& Target)
{
	for (auto mi : items)
		Target.draw(mi.text);
}
//----------------------------
//			TMainMenuState
//----------------------------


TMainMenuState::TMainMenuState()
{
	assert(mFont.loadFromFile("c:/windows/fonts/consola.ttf"));

	float x(100.0f), y(100.0f);
	mMenu.ObjectHandler(this);	// Yeeees, I know, it's must be template for type-safety, but... I hate template: bloat exe, increase compilation time and poor IDE performance...
	
	mMenu += TMenuItem().Text(L"Ѕродилка")
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
	mMenu.Draw(Win);
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
	
	// —просим у юзера каким размеров карту ему хочетс€.
	HWND hDlg;
	MSG msg;
	BOOL ret;
	LRESULT lres;
	// ƒиалог -- IDD_NEW_MAP_DIALOG //
	hDlg = CreateDialogParam(gHinstance, MAKEINTRESOURCE(IDD_NEW_MAP_DIALOG), Win.getSystemHandle(), DialogProc, 0);
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
	// —читаем из диалога пользовательский ввод.
	TMapParams Param;
	assert( 0 != GetDlgItemText(hDlg, IDC_EDIT_FILENAME, const_cast<LPWSTR>(Param.FileName.c_str()), static_cast<int>(Param.FileName.size())) );
	BOOL ok;
	Param.w = GetDlgItemInt(hDlg, IDC_EDIT_WIDTH, &ok, /*signed =*/FALSE);
	assert(ok == TRUE);
	assert(Param.w > 0);
	assert(Param.w < 65535);
	Param.h = GetDlgItemInt(hDlg, IDC_EDIT_HEIGHT, &ok, /*signed =*/FALSE);
	assert(ok == TRUE);
	assert(Param.h > 0);
	assert(Param.h < 65535);
	Param.PrevalentTileType = TileTypeFromInt( (int)SendMessage(hWndComboBox, CB_GETCURSEL, 0, 0) );

	DestroyWindow(hDlg);


	//Param.h = 16;
	//Param.w = 16;
	//Param.PrevalentTileType = TileType::Graund;
	
	self.mState->GotoMapEditor_CreateMap(Param);
	
	for (int x = 0; x < Param.w; ++x)
	{
		TCoord2Int c;
		c.x = x;
		c.y = 0;
		self.mState->mMapEditorState->mTileMap.mMap[c] = TileType::Water;
		c.y = Param.h - 1;
		self.mState->mMapEditorState->mTileMap.mMap[c] = TileType::Water;
	}
	for (int y = 1; y < (Param.h - 1); ++y)
	{
		TCoord2Int c;
		c.y = y;
		c.x = 0;
		self.mState->mMapEditorState->mTileMap.mMap[c] = TileType::Water;
		c.x = Param.w - 1;
		self.mState->mMapEditorState->mTileMap.mMap[c] = TileType::Water;
	}
}



void TMainMenuState::OnLoadMap(void *This)
{
	TMainMenuState &self = *((TMainMenuState*)This);
	
	static const int FileName_BufSize = 4 * 1024;
	wchar_t FileName[FileName_BufSize] = { 0 };
	wchar_t FilePath[FileName_BufSize] = { 0 };
	int PathLen = GetModuleFileNameW(gHinstance, FilePath, FileName_BufSize);
	if (int er = GetLastError())
	{
		ShowLastErr(er);
		return;
	}
	// Ёх, где же ты удобна€ VCL GetFilePath
	*wcsrchr(FilePath, L'\\') = L'\0';
	
	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = FileName;
	ofn.nMaxFile = FileName_BufSize;
	ofn.lpstrInitialDir = FilePath;

	if (GetOpenFileNameW(&ofn))
	{
		MessageBoxW(NULL, ofn.lpstrFile, L"File Name", MB_OK);
		GameState.GotoMapEditor_LoadMap(ofn.lpstrFile);
	}

}
