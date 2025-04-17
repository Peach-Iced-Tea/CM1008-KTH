#pragma once
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "entity.h"
#include "vmath.h"

typedef enum {
    IDLE, RUNNING, JUMPING, FALLING, ROTATING, FLYING
} State;

typedef struct player Player;

Player *createPlayer(Vec2 position, SDL_Texture *pTexture);

bool playerHandleInput(Player *pPlayer);

void playerUpdateState(Player *pPlayer, float deltaTime);

Vec2 playerUpdatePosition(Player *pPlayer, float deltaTime);

int playerCheckCollision(Player *pPlayer, Entity *pEntity);

bool playerSetState(Player *pPlayer, int newState);

void playerSetRadius(Player *pPlayer, float radius);

void playerSetReferenceAngle(Player *pPlayer, float newAngle);

int playerGetState(Player *pPlayer);

Entity *playerGetBody(Player const *pPlayer);

bool playerGetMouseClick(Player const *pPlayer);

void destroyPlayer(Player *pPlayer);