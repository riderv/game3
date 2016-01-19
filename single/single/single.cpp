// single.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "single.h"

#include "main_window.h"

void MainLoop();

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MainLoop();
	return 0;
}

#include "main_window_impl.h"
