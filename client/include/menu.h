#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdbool.h>

#include "renderWindow.h"
#include "input.h"
#include "networkData.h"

typedef struct menu Menu;

Menu *createMenu(SDL_Renderer *renderer);

/*
Displays the main menu of the game, and waits for user input.
*/
bool mainMenu(Menu *menu, RenderWindow *window, IPaddress *serverAddress);

/*
Use this function to destroy the given Menu pointer and free up memory.
*/
void destroyMenu(Menu *menu);