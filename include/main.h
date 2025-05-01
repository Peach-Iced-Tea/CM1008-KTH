#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <stdbool.h>

#include "vmath.h"
#include "physics.h"
#include "entity.h"
#include "input.h"
#include "camera.h"
#include "renderWindow.h" 
#include "utils.h"
#include "player.h"
#include "networkData.h"
#include "client.h"
#include "menu.h"
#include "mapParser.h"

typedef struct {
    RenderWindow *pWindow;
    Camera *pCamera;
    Player *players[MAX_PLAYERS];
    SDL_Texture *pGrassTexture;
    DynamicArray *pPlatforms;
    Input *pInput;
    Menu *pMenu;
    Client *pClient;

    ClientMap *pMap;
    DynamicArray *pHitforms;

} Game;