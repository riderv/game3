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
	GameState.GotoPlay_LoadMap(L"d:\\_pro\\game3\\maps\\newMap2");
	//sf::View v = win.getView();
	//v.zoom(0.5f);
	//v.setCenter( float(win.getSize().x/4), float(win.getSize().y/4));
	//win.setView(v);
	// Start game loop
	Win.setVerticalSyncEnabled(true);
	sf::Clock clock;
	sf::Event Event;

	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	LARGE_INTEGER t1, t2;
	QueryPerformanceCounter(&t1);
	LARGE_INTEGER ElapsedMicroseconds;
	while (Win.isOpen())
	{
		// Process events
		while (Win.pollEvent(Event))
		{
			// Close window : exit
			GameState.PoolEvent(Event);
			if(GameState.IsClosed )
				Win.close();

			if (Event.type == sf::Event::Resized)
			{
				GameState.OnResize();
			}
		}
	//	GameState.DT = clock.getElapsedTime().asMilliseconds();
		QueryPerformanceCounter(&t2);
		ElapsedMicroseconds.QuadPart = t2.QuadPart - t1.QuadPart;
		// We now have the elapsed number of ticks, along with the
		// number of ticks-per-second. We use these values
		// to convert to the number of elapsed microseconds.
		// To guard against loss-of-precision, we convert
		// to microseconds *before* dividing by ticks-per-second.
		ElapsedMicroseconds.QuadPart *= 1000000;
		ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
		// micro to seconds
		GameState.DTmcs = ElapsedMicroseconds.QuadPart;
		t1 = t2;
		GameState.Simulate();
		Win.clear();
		GameState.Draw();
		Win.display();
			
		//i32 elapsed = clock.getElapsedTime().asMilliseconds();
		//if (elapsed < (1000 / 60)) {
		//	Sleep((1000 / 60) - elapsed);			
		//}
		clock.restart();
	}	
}
