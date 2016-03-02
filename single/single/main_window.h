#pragma once

#include "stdafx.h"
#include "game_state.h"
#include "tile_types.h"

extern char g_GameStateMemorySpace[sizeof(TGameState)];
extern TGameState & GameState;

extern char g_WindowMemotySpace[sizeof(sf::RenderWindow)];
extern sf::RenderWindow & Win;

void MainLoop()
{
	new(g_WindowMemotySpace) sf::RenderWindow(sf::VideoMode(800, 600, 32), "Candies killer.");
	struct TFinalyWin {
		~TFinalyWin() { Win.~RenderWindow(); }
	} FinWin;
	
	new(g_GameStateMemorySpace) TGameState();
	struct TFinalyGameState {
		~TFinalyGameState() { GameState.~TGameState(); }
	}FinGameState;
	
	assert(TTileType::Count == sizeof(TileType_Names) / sizeof(TileType_Names[0]));
	//GameState.GotoPlay_LoadMap(L"d:\\_pro\\game3\\maps\\newMap3");

	Win.setVerticalSyncEnabled(true);
	//Win.setFramerateLimit(60);
	sf::Clock clock;
	int FrameCount = 0;
	sf::Event Event;

	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	LARGE_INTEGER t1, t2;
	QueryPerformanceCounter(&t1);
	ui64 DeltaMcrSecs;

	while (Win.isOpen())
	{
		while (Win.pollEvent(Event))
		{
			GameState.PoolEvent(Event);
			if(GameState.mIsClosed )
				Win.close();

			if (Event.type == sf::Event::Resized)
				GameState.OnResize();
		}
		QueryPerformanceCounter(&t2);
		DeltaMcrSecs = t2.QuadPart - t1.QuadPart;
		//if (DeltaMcrSecs < 33000)
		//	Sleep((DWORD)(33000 - DeltaMcrSecs) / 1000);

		if(DeltaMcrSecs > 33000)
			DeltaMcrSecs = 33000; // замедление лучше рывков

		GameState.DTs = float(double(DeltaMcrSecs) / 1000000);
		t1 = t2;
		
		GameState.Simulate();
		Win.clear();
		GameState.Draw();
		Win.display();
			
		if (clock.getElapsedTime().asMilliseconds() >= 1000) // можно было бы clock использовать для dt, но раз уж сделал, пусть будет более точное 
		{
			char buf[128] = "Candies killer. fps:";
			_itoa_s(FrameCount, buf + strlen(buf), 128-strlen(buf)-1, 10);
			Win.setTitle(buf);
			clock.restart();
			FrameCount = 0;
		}
		else
			FrameCount++;
	}	
}
