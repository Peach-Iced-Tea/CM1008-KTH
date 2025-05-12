#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdbool.h>

#include "renderWindow.h"
#include "input.h"
#include "networkData.h"

typedef struct menu Menu;

Menu *createMenu(RenderWindow *window);

bool mainMenu(Menu *menu, RenderWindow *window, IPaddress *serverAddress);

void destroyMenu(Menu *menu);