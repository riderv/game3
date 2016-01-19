#pragma once

#include "stdafx.h"
#include "game_state.h"

extern char g_GameStateSpace[sizeof(TGameState)];
extern TGameState * gpGameState;
extern sf::RenderWindow *gpWin;

void MainLoop()
{

	sf::RenderWindow win(sf::VideoMode(800, 600, 32), "SFML Graphics");
	gpWin = &win;
	gpGameState = new (g_GameStateSpace)TGameState();

	// Start game loop
	while (win.isOpen())
	{
		// Process events
		sf::Event Event;
		while (win.pollEvent(Event))
		{
			// Close window : exit
			gpGameState->PoolEvent(Event);
			if(gpGameState->IsClosed() )
				win.close();
		}
		gpGameState->Simulate();
		// Clear the screen (fill it with black color)
		win.clear();
		gpGameState->Draw();
		// Display window contents on screen
		win.display();
	}	
}
