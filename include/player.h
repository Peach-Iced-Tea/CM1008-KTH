#pragma once
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "entity.h"
#include "vmath.h"

typedef struct player Player;

Player *createPlayer(Vec2 position, SDL_Texture *pTexture);

bool playerHandleInput(Player *pPlayer);

void playerUpdateState(Player *pPlayer, float deltaTime);

bool playerUpdatePosition(Player *pPlayer, float deltaTime);

bool playerCheckCollision(Player *pPlayer, Entity *pEntity);

void playerSetFalling(Player *pPlayer);

Entity *pPlayerGetBody(Player const *pPlayer);

void destroyPlayer(Player *pPlayer);