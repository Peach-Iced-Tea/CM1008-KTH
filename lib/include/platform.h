#pragma once
#include "entity.h"
#include "vmath.h"

#define MAX_MOVE_PLATFORMS 8
#define PLATFORM_SPEED 2.0f
#define MAX_PLATFORM_SIZE 16

typedef enum PlatformDirection {
    PLATFORM_LEFT, PLATFORM_RIGHT
} PlatformDirection;

typedef struct movablePlatform MovablePlatform;

MovablePlatform *createPlatform(Vec2 position);

void platformMove(MovablePlatform *platform);

Entity platformGetBody(MovablePlatform const *platform, int index);

Hitbox *platformGetHitbox(MovablePlatform const *platform, int index);

Vec2 platformGetVelocity(MovablePlatform const *platform);

int platformGetSize(MovablePlatform const *platform);

void destroyPlatform(MovablePlatform *platform);