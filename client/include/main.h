#pragma once
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#include "vmath.h"
#include "entity.h"
#include "input.h"
#include "renderWindow.h" 
#include "utils.h"
#include "player.h"
#include "client.h"
#include "menu.h"
#include "mapParser.h"
#include "mouse.h"

typedef struct {
    RenderWindow *pWindow;
    Camera *pCamera;
    Player *players[MAX_PLAYERS];
    Input *pInput;
    Menu *pMenu;
    Client *pClient;
    ClientMap *pMap;
    DynamicArray *pHitforms;
    Crosshair *pCrosshair;
} Game;