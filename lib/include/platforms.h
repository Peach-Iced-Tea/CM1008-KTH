#pragma once
#include <SDL2/SDL.h>

#include "entity.h"
#include "vmath.h"

#define MAX_MOVE_PLATFORMS 8
#define PLATFORM_SPEED 2.0f
#define MAX_PLATFORM_SIZE 16

typedef struct movablePlatform MovablePlatform;

MovablePlatform *createPlatform();

void initMovablePlatforms(SDL_Renderer *pWindow,MovablePlatform *pPlatformOne);

void movePlatforms(MovablePlatform *pPlatformOne);

Entity *platformGetEntity(MovablePlatform *pPlatform, int index);

Vec2 platformGetVelocity(MovablePlatform *pPlatform);

void destroyPlatform(MovablePlatform *pPlatform);