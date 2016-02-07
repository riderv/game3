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

	//sf::View v = win.getView();
	//v.zoom(0.5f);
	//v.setCenter( float(win.getSize().x/4), float(win.getSize().y/4));
	//win.setView(v);
	// Start game loop
	sf::Clock clock;
	while (Win.isOpen())
	{
		// Process events
		sf::Event Event;
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
		GameState.Simulate();
		// Clear the screen (fill it with black color)
		Win.clear();
		GameState.Draw();
		// Display window contents on screen
		Win.display();
		
		i32 elapsed = clock.getElapsedTime().asMilliseconds();
		if (elapsed < (1000 / 60)) {
			Sleep((1000 / 60) - elapsed);			
		}
		clock.restart();
	}	
}
