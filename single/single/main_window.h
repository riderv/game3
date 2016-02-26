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
	new(g_WindowMemotySpace) sf::RenderWindow(sf::VideoMode(800, 600, 32), "Brodilka");
	struct TFinWin {
		~TFinWin() { Win.~RenderWindow(); }
	} FinWin;
	
	new(g_GameStateMemorySpace) TGameState();
	struct TFinGameState {
		~TFinGameState() { GameState.~TGameState(); }
	}FinGameState;
	
	assert(TTileType::Count == sizeof(TileType_Names) / sizeof(TileType_Names[0]));
	//GameState.GotoPlay_LoadMap(L"d:\\_pro\\game3\\maps\\newMap3");

	Win.setVerticalSyncEnabled(true);
	//Win.setFramerateLimit(0);
	sf::Clock clock;
	int FrameCount = 0;
	sf::Event Event;

	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	LARGE_INTEGER t1, t2;
	QueryPerformanceCounter(&t1);
//	LARGE_INTEGER ElapsedMicroseconds;
	ui64 DeltaMcrSecs;

	while (Win.isOpen())
	{
		// Process events
		while (Win.pollEvent(Event))
		{
			// Close window : exit
			GameState.PoolEvent(Event);
			if(GameState.mIsClosed )
				Win.close();

			if (Event.type == sf::Event::Resized)
			{
				GameState.OnResize();
			}
		}
		QueryPerformanceCounter(&t2);
		DeltaMcrSecs = t2.QuadPart - t1.QuadPart;
		//if (DeltaMcrSecs < 33000)
		//{
		//	Sleep((DWORD)(33000 - DeltaMcrSecs) / 1000);
		//}
		if(DeltaMcrSecs > 33000)
			DeltaMcrSecs = 33000;

		GameState.DTs = float(double(DeltaMcrSecs) / 1000000);
		t1 = t2;
		
		
		GameState.Simulate();
		Win.clear();
		GameState.Draw();
		Win.display();
			
		//i32 elapsed = clock.getElapsedTime().asMilliseconds();
		//if (elapsed < (1000 / 60)) {
		//	Sleep((1000 / 60) - elapsed);			
		//}
		if (clock.getElapsedTime().asMilliseconds() >= 1000)
		{
			char buf[256];
			_itoa_s(FrameCount, buf, 10);
			Win.setTitle(buf);
			clock.restart();
			FrameCount = 0;
		}
		FrameCount++;


	}	
}
