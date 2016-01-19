// single.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "single.h"

#include "main_window.h"
#include "game_state.h"

void MainLoop();

HINSTANCE gHinstance;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	gHinstance = hInstance;
	INITCOMMONCONTROLSEX i = { 0 };
	i.dwSize = sizeof(i);
	i.dwICC = ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&i);
	MainLoop();
	return 0;
}

#include "main_window_impl.h"
#include "game_state_impl.h"
