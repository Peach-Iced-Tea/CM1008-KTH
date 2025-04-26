#pragma once
#include <SDL2/SDL.h>
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

#define MAX_PLAYERS 2

typedef struct {
    RenderWindow *pWindow;
    Camera *pCamera;
    Player *players[MAX_PLAYERS];
    SDL_Texture *pGrassTexture;
    DynamicArray *pPlatforms;
    Input *pInput;
} Game;