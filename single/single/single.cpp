// single.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "single.h"

#include "TLog.h"
#include "main_window.h"
#include "tile_map.h"
#include "game_state.h"

void MainLoop();

HINSTANCE gHinstance;
TLog Log;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	union{
		int i;
		float f;
	}u;
	u.i = 123;
	u.f = 123.123f;

	gHinstance = hInstance;
	INITCOMMONCONTROLSEX i = { 0 };
	i.dwSize = sizeof(i);
	i.dwICC = ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&i);
	Log.Init();
	MainLoop();
	
	//_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
	//_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_WNDW);
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtDumpMemoryLeaks();
	return 0;
}

#include "main_menu_impl.h"
#include "main_window_impl.h"
#include "game_state_impl.h"
#include "map_editor_impl.h"
#include "tile_map_impl.h"