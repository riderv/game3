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
#include "sys.h"
#include "exception.h"
//----------------------------
//			TMenuItem
//----------------------------
template <class T>
inline TMenuItem& TMenuItem::OnKey( sf::Keyboard::Key Key, T* object, void( *callback_ptr )(T* object) )
{
	assert(callback_ptr);
	this->Key = Key;
	this->object = object;
	this->callback = reinterpret_cast<callback_t>(callback_ptr);
	return *this;
}
inline bool TMenuItem::ProcessKey(sf::Keyboard::Key Key)
{
	if (Key == sf::Keyboard::Unknown)
		return false;

	if (Key == this->Key) {
		callback(object);
		return true;
	}
	return false;
}

inline void TMenu::ProcessKey(sf::Keyboard::Key Key)
{
	for (auto mi : items)
		if (mi.ProcessKey(Key))
			break;
}

inline void TMenu::Draw(sf::RenderTarget& Target)
{
	for (auto mi : items)
		Target.draw(mi.mText);
}
//----------------------------
//			TMainMenuState
//----------------------------

TMainMenuState::~TMainMenuState()
{
}


TMainMenuState::TMainMenuState(TGameState* pGameState)
	: mState(pGameState)
{
	float x(100.0f), y(100.0f);

	mMenu += TMenuItem().Text(L"Бродилка")
		.Font(GameState.mFont).Pos(x, y).CharSize(30);
	y += 60;

	mMenu += TMenuItem().Text("1) Generate and edit map...")
		.Font(GameState.mFont).Pos(x, y).CharSize(20)
		.OnKey(sf::Keyboard::Num1, this, &TMainMenuState::OnGenMap);
	y += 30;

	mMenu += TMenuItem().Text("2) Load map and edit...")
		.Font(GameState.mFont).Pos(x, y).CharSize(20)
		.OnKey(sf::Keyboard::Num2, this, &TMainMenuState::OnLoadMap);
	
	y += 30;
	mMenu += TMenuItem().Text("3) Load map and play.")
		.Font(GameState.mFont).Pos(x, y).CharSize(20)
		.OnKey(sf::Keyboard::Num3, this, &TMainMenuState::OnLoadAndPlay);

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

int DialogResult = IDCANCEL;

static int __stdcall BrowseNotify(HWND hWnd, UINT iMessage, LPARAM wParam, LPARAM lParam)
{
	if (iMessage == BFFM_INITIALIZED)
	{
		// Шешил в файлик сохранять предыдущие настройки, 
		// в частности куда последний раз создавали карту,
		// и попал на несколько часов: 
		// не знал, что работать с текстовыми юникод-файлами
		// в винде такая голованя боль.
		// TODO: ПЕРЕДЕЛАТЬ НА БД ИЛИ БИНАРНЫЙ ФАЙЛ или xml
		std::wstring fn = GetExePatch() + L"\\last_map_dir";
#pragma warning(disable:4996)
		FILE * F = _wfopen( fn.c_str(), L"rt, ccs=UTF-16LE");
#pragma warning(default:4996)
		if (!F)
			return 1;
		struct TFClose { FILE *f;  ~TFClose() { fclose(f); } } FClose = { F };
		wchar_t str[1024 * 4];
		const wchar_t ptrn[] = L"last_map_dir=";
		bool found = false;
		while (!feof(F))
		{
			if (NULL == fgetws(str, 1024 * 4, F))
				return 1;

			if (0 == wcsncmp(ptrn, str, wcslen(ptrn)))
			{
				if ( wcslen(str) > wcslen(ptrn) ){
					wcsncpy_s(str,wcslen(str), str + wcslen(ptrn), wcslen(str) - wcslen(ptrn)-1);
					found = true;
				}	
				break;
			}
		}
		if(found)
			SendMessageW(hWnd, BFFM_SETSELECTION, 1, (LPARAM)str);    // Set initial folder
		return 1;
	}
	return 0;
}

INT_PTR __stdcall DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) /* more compact, each "case" through a single line, easier on the eyes */
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			DialogResult = IDOK;
		case IDCANCEL:
			PostQuitMessage(0);
			return TRUE;
		case IDC_BUTTON_BROWSE:
			{
				BROWSEINFO bi = { 0 };
				bi.lpszTitle = L"Select directory";
				bi.lpfn = BrowseNotify;
				LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
				if (pidl)
				{
					wchar_t tszPath[MAX_PATH] = L"\0";
					if (SHGetPathFromIDList(pidl, tszPath) == TRUE)
					{
						std::wstring strPath = tszPath;
						SetDlgItemTextW(hDlg, IDC_STATIC_FullName, strPath.c_str());
					}
					CoTaskMemFree(pidl);
				}
			}
		return TRUE;
		}
		break;

	case WM_CLOSE:
		PostQuitMessage(0);
		return TRUE;
	}
	return FALSE;
}


void TMainMenuState::OnGenMap(TMainMenuState *This)
{
	HWND hDlg = NULL;
	try {
		// Спросим у юзера каким размеров карту ему хочется.
		MSG msg;
		BOOL ret;
		LRESULT lres;
		// Диалог -- IDD_NEW_MAP_DIALOG //
		hDlg = CreateDialogParam(gHinstance, MAKEINTRESOURCE(IDD_NEW_MAP_DIALOG), Win.getSystemHandle(), DialogProc, 0);
		assert(NULL != hDlg);

		HWND hWndComboBox = GetDlgItem(hDlg, IDC_COMBO_TILES);
		assert(NULL != hWndComboBox);

		for (int i = 0; i < int(TTileType::Count); ++i)
		{
			LRESULT res = SendMessageA(hWndComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)TileType_Names[i]);
			assert(res == i);
		}
		lres = SendMessage(hWndComboBox, CB_SETCURSEL, /*index=*/0, 0);

		std::wstring ExePath = GetExePatch();
		SetDlgItemTextW(hDlg, IDC_STATIC_FullName, const_cast<LPCWSTR>(ExePath.c_str()));


		ShowWindow(hDlg, SW_SHOW);
		while ((ret = GetMessage(&msg, 0, 0, 0)) != 0)
		{
			if (!IsDialogMessage(hDlg, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		if (DialogResult != IDOK) {
			DestroyWindow(hDlg);
			hDlg = NULL;
			return;
		}
		// Считаем из диалога пользовательский ввод.	
		const int Dir_Len = 1024 * 4;
		wchar_t FileName[Dir_Len] = { 0 };
		wchar_t Dir[Dir_Len] = { 0 };
		TMapParams Param;
		assert(0 != GetDlgItemTextW(hDlg, IDC_EDIT_FILENAME,
			FileName,
			Dir_Len));
		assert(0 != GetDlgItemTextW(hDlg, IDC_STATIC_FullName,
			Dir,
			Dir_Len));
		assert(0 < wcslen(FileName));
		assert(0 < wcslen(Dir));
		Param.FileName = Dir;
		if (!(Dir[wcslen(Dir) - 1] == L'\\' || Dir[wcslen(Dir) - 1] == L'/'))
			Param.FileName += L'\\';
		Param.FileName += FileName;
		BOOL ok;
		Param.w = GetDlgItemInt(hDlg, IDC_EDIT_WIDTH, &ok, /*signed =*/FALSE);
		assert(ok == TRUE);
		assert(Param.w > 0);
		assert(Param.w <= Max<ui16>());
		Param.h = GetDlgItemInt(hDlg, IDC_EDIT_HEIGHT, &ok, /*signed =*/FALSE);
		assert(ok == TRUE);
		assert(Param.h > 0);
		assert(Param.h <= Max<ui16>());
		Param.DefaultTileType = TileTypeFromInt((int)SendMessage(hWndComboBox, CB_GETCURSEL, 0, 0));

		DestroyWindow(hDlg);
		hDlg = NULL;


		//Param.h = 16;
		//Param.w = 16;
		//Param.DefaultTileType = TileType::Graund;


		for (int x = 0; x < Param.w; ++x)
		{
			This->mState->mMapEditorState->TileMap().Set(x, 0, TTileType::Water);
			This->mState->mMapEditorState->TileMap().Set(x, Param.h - 1, TTileType::Water);
		}
		for (int y = 1; y < (Param.h - 1); ++y)
		{
			This->mState->mMapEditorState->TileMap().Set(0, y, TTileType::Water);
			This->mState->mMapEditorState->TileMap().Set(Param.w - 1, y, TTileType::Water);
		}
		//This->mState->mMapEditorState->TileMap().Set(3, 3, TTileType::Water);
		This->mState->GotoMapEditor_CreateMap(Param);
		return;
	}
	catch (const TException &e)
	{
		if(hDlg)
			DestroyWindow(hDlg);
		Log( e.msg );
		MessageBoxW(0, e.msg.c_str(), L"ERROR", MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(0, "TMainMenuState::OnGenMap", "Unknown exception", MB_ICONERROR);
		Log( L"Unknown exception in TMainMenuState::OnGenMap" );
	}
}



void TMainMenuState::OnLoadMap( TMainMenuState *This )
{
	try {
		static const int FileName_BufSize = 4 * 1024;
		wchar_t FileName[FileName_BufSize] = { 0 };
		wchar_t FilePath[FileName_BufSize] = { 0 };
		int PathLen = GetModuleFileNameW(gHinstance, FilePath, FileName_BufSize);
		if (int er = GetLastError())
		{
			ShowLastErr(er);
			return;
		}
		// Эх, где же ты удобная VCL
		*wcsrchr(FilePath, L'\\') = L'\0';

		OPENFILENAMEW ofn = { 0 };
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = FileName;
		ofn.nMaxFile = FileName_BufSize;
		ofn.lpstrInitialDir = FilePath;

		if (GetOpenFileNameW(&ofn))
		{
			std::wstring w(1024, L'\0');
			GetWindowTextW(Win.getSystemHandle(), const_cast<LPWSTR>(w.c_str()), static_cast<int>(w.size()));
			Win.setTitle("Loading map...");
			Win.setTitle(w.c_str());
			GameState.GotoMapEditor_LoadMap(ofn.lpstrFile);
		}
	}
	catch (const TException &e)
	{
		MessageBoxW(0, e.msg.c_str(), L"ERROR", MB_ICONEXCLAMATION);
		Log( L"TMainMenuState::OnLoadMap catch exception with message" + e.msg );
	}
	catch (...)
	{
		MessageBoxA(0, "TMainMenuState::OnLoadMap", "Unknown exception", MB_ICONERROR);
		Log( L"TMainMenuState::OnLoadMap catch unknown exception" );
	}
}

void TMainMenuState::OnLoadAndPlay( TMainMenuState *This )
{
	try {
		static const int FileName_BufSize = 4 * 1024;
		wchar_t FileName[FileName_BufSize] = { 0 };
		wchar_t FilePath[FileName_BufSize] = { 0 };
		int PathLen = GetModuleFileNameW(gHinstance, FilePath, FileName_BufSize);
		if (int er = GetLastError())
		{
			ShowLastErr(er);
			return;
		}
		// Эх, где же ты удобная VCL
		*wcsrchr(FilePath, L'\\') = L'\0';

		OPENFILENAMEW ofn = { 0 };
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = FileName;
		ofn.nMaxFile = FileName_BufSize;
		ofn.lpstrInitialDir = FilePath;

		if (GetOpenFileNameW(&ofn))
		{
			std::wstring w(1024, L'\0');
			GetWindowTextW(Win.getSystemHandle(), const_cast<LPWSTR>(w.c_str()), static_cast<int>(w.size()));
			Win.setTitle("Loading map for playing...");
			GameState.GotoPlay_LoadMap(ofn.lpstrFile);
			Win.setTitle(w.c_str());
		}
		return;
	}
	catch (const TException& E)
	{
		MessageBoxW(0, E.msg.c_str(), L"Error: TMainMenuState::OnLoadAndPlay", MB_ICONERROR);
		Log( L"TMainMenuState::OnLoadAndPlay catch exception" + E.msg );
	}
	catch (...)
	{
		MessageBoxA(0, "TMainMenuState::OnLoadAndPlay", "Unknown exception", MB_ICONERROR);
		Log( L"TMainMenuState::OnLoadAndPlay catch unknown exception" );
	}
	GameState.GotoMainMenu();
}