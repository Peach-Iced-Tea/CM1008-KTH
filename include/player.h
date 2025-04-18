#pragma once
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "entity.h"
#include "vmath.h"

typedef enum {
    IDLE, RUNNING, JUMPING, FALLING, ROTATING, FLYING
} PlayerState;

typedef struct player Player;

Player *createPlayer(Vec2 position, SDL_Texture *pTexture);

bool playerHandleInput(Player *pPlayer);

void playerUpdateState(Player *pPlayer, float deltaTime);

void playerUpdatePosition(Player *pPlayer, float deltaTime);

int playerCheckCollision(Player *pPlayer, Entity *pEntity);

bool playerSetState(Player *pPlayer, int newState);

Entity *playerGetBody(Player const *pPlayer);

int playerGetState(Player const *pPlayer);

void destroyPlayer(Player *pPlayer);