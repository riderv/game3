#pragma once

#include "main_window.h"
#include "game_state.h"

char g_GameStateMemorySpace[sizeof(TGameState)];
TGameState & GameState = *(TGameState*)g_GameStateMemorySpace;

char g_WindowMemotySpace[sizeof(sf::RenderWindow)];
sf::RenderWindow & Win = *(sf::RenderWindow*)g_WindowMemotySpace;
