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


inline TMainMenuState::TMainMenuState()
{
	float x(100.0f), y(100.0f);

	mMenu += TMenuItem().Text(L"Истребитель конфет")
		.Font(GameState.mFont).Pos(x, y).CharSize(30);
	y += 60;

	mMenu += TMenuItem().Text(L"1) Создать карту")
		.Font(GameState.mFont).Pos(x, y).CharSize(20)
		.OnKey(sf::Keyboard::Num1, this, &TMainMenuState::OnGenMap);
	y += 30;

	mMenu += TMenuItem().Text(L"2) Загрузить карту и редактировать")
		.Font(GameState.mFont).Pos(x, y).CharSize(20)
		.OnKey(sf::Keyboard::Num2, this, &TMainMenuState::OnLoadMap);
	
	y += 30;
	mMenu += TMenuItem().Text(L"3) Загрузить карту и играть")
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
		// Решил в файлик сохранять предыдущие настройки, 
		// в частности куда последний раз создавали карту.
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
	struct TDialog {
		HWND Handle;
		~TDialog() { if( Handle ) DestroyWindow( Handle ); }
	} Dialog{ NULL };

	try {
		// Спросим у юзера каким размеров карту ему хочется.
		// (создание простенького диалога из ресурсов).
		MSG msg;
		BOOL ret;
		LRESULT lres;
		// Диалог -- IDD_NEW_MAP_DIALOG //
		Dialog.Handle = CreateDialogParam(gHinstance, MAKEINTRESOURCE(IDD_NEW_MAP_DIALOG), Win.getSystemHandle(), DialogProc, 0);
		CHECK_TRUE(NULL != Dialog.Handle, L"CreateDialogParam failed.");

		// Заполнить комбобокс названием тайлов.
		HWND hWndComboBox = GetDlgItem(Dialog.Handle, IDC_COMBO_TILES);
		CHECK_TRUE(NULL != hWndComboBox, L"GetDlgItem(Dialog.Handle, IDC_COMBO_TILES) failed, ");

		for (int i = 0; i < int(TTileType::Count); ++i)
		{
			LRESULT res = SendMessageA(hWndComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)TileType_Names[i]);
			assert(res == i);
		}
		lres = SendMessage(hWndComboBox, CB_SETCURSEL, /*index=*/0, 0);

		std::wstring ExePath = GetExePatch();
		SetDlgItemTextW(Dialog.Handle, IDC_STATIC_FullName, const_cast<LPCWSTR>(ExePath.c_str()));


		ShowWindow( Dialog.Handle, SW_SHOW );
		UpdateWindow( Dialog.Handle );
		while ((ret = GetMessage(&msg, 0, 0, 0)) != 0)
		{
			if (!IsDialogMessage(Dialog.Handle, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		if (DialogResult != IDOK)
			return;

		// Читаем из диалога пользовательский ввод.	
		const int Dir_Len = 1024 * 4;
		wchar_t FileName[Dir_Len] = { 0 };
		wchar_t Dir[Dir_Len] = { 0 };
		TMapParams Param;

		CHECK_TRUE(0 != GetDlgItemTextW(Dialog.Handle,IDC_EDIT_FILENAME,FileName,Dir_Len),
			L"GetDlgItemTextW(Dialog.Handle,IDC_EDIT_FILENAME,....) failed.");

		CHECK_TRUE(0 != GetDlgItemTextW(Dialog.Handle, IDC_STATIC_FullName,Dir,Dir_Len),
			L"GetDlgItemTextW( Dialog.Handle, IDC_STATIC_FullName,....");

		CHECK_TRUE(0 < wcslen(FileName), L"Имя файла не задано.");
		CHECK_TRUE(0 < wcslen(Dir), L"Путь к файлу не указан.");
		
		Param.FileName = Dir;
		// Add traling b-slash if needed.
		if (!(Dir[wcslen(Dir) - 1] == L'\\' || Dir[wcslen(Dir) - 1] == L'/'))
			Param.FileName += L'\\';
		Param.FileName += FileName;
		
		BOOL ok;
		Param.w = GetDlgItemInt( Dialog.Handle, IDC_EDIT_WIDTH, &ok, /*signed =*/FALSE);
		CHECK_TRUE(ok == TRUE, L"GetDlgItemInt( Dialog.Handle, IDC_EDIT_WIDTH....) failed.");
		CHECK_TRUE(Param.w > 0, L"Ширина карты должна быть больше нуля");
		CHECK_TRUE(Param.w <= Max<ui16>(), L"Ширина карты должна быть <= 65535");
		
		Param.h = GetDlgItemInt( Dialog.Handle, IDC_EDIT_HEIGHT, &ok, /*signed =*/FALSE);
		CHECK_TRUE(ok == TRUE, L"GetDlgItemInt( Dialog.Handle, IDC_EDIT_HEIGHT....) failed.");
		CHECK_TRUE(Param.h > 0, L"Высота карты должна быть больше нуля.");
		CHECK_TRUE(Param.h <= Max<ui16>(), L"Высота карты должна быть <= 65535");

		int DefTileType = static_cast<int>(  SendMessage( hWndComboBox, CB_GETCURSEL, 0, 0 )  );
		CHECK_TRUE( DefTileType >= 0 &&
					DefTileType <= TTileType::Count,
					L"Неверный базовый тип тайла (default tile type)."
					);
		Param.DefaultTileType = TileTypeFromInt( DefTileType );

		GameState.GotoMapEditor_CreateMap(Param);
	
		// Зальём водой по краям
		for (int x = 0; x < Param.w; ++x)
		{
			GameState.mMapEditorState.TileMap().Set(x, 0, TTileType::Water);
			GameState.mMapEditorState.TileMap().Set(x, Param.h - 1, TTileType::Water);
		}
		for (int y = 1; y < (Param.h - 1); ++y)
		{
			GameState.mMapEditorState.TileMap().Set(0, y, TTileType::Water);
			GameState.mMapEditorState.TileMap().Set(Param.w - 1, y, TTileType::Water);
		}
		//This->mState->mMapEditorState->TileMap().Set(3, 3, TTileType::Water);// для отладки
		return;
	}
	catch (const TException &e)
	{
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
		// Запросить имя файла
		static const int FileName_BufSize = 4 * 1024;
		wchar_t FileName[FileName_BufSize] = { 0 };
		wchar_t FilePath[FileName_BufSize] = { 0 };
		int PathLen = GetModuleFileNameW(gHinstance, FilePath, FileName_BufSize);
		if (int er = GetLastError())
		{
			ShowLastErr(er);
			Log( L"TMainMenuState::OnLoadMap, GetModuleFileNameW failed. " + LastErrStr( er ) );
			GameState.GotoMainMenu();
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
			return;
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
	GameState.GotoMainMenu();
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
			const int t_len = 1024;
			wchar_t prev_title[t_len] = { 0 };
			GetWindowTextW(Win.getSystemHandle(), prev_title, t_len);
			Win.setTitle("Loading map for playing...");
			GameState.GotoPlay_LoadMap(ofn.lpstrFile);
			Win.setTitle( prev_title );
			return;
		}		
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