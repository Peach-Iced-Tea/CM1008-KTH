#pragma once
#include <SDL2/SDL.h>
#include <string.h>
#include <stdbool.h>

#include "renderWindow.h"
#include "input.h"
#include "networkData.h"

#define IP_MAX_LENGTH 16

typedef struct menu Menu;

Menu *createMenu(SDL_Renderer *renderer);

/*
Displays the main menu of the game, and waits for user input.
*/
bool mainMenu(Menu *menu, RenderWindow *window, char *serverAddress);

/*
Use this function to destroy the given Menu pointer and free up memory.
*/
void destroyMenu(Menu *menu);