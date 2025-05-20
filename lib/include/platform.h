#pragma once
#include "vmath.h"
#include "entity.h"

typedef enum PlatformType {
    PLATFORM_FLAT, PLATFORM_CONVEX
} PlatformType;

typedef struct movablePlatform MovablePlatform;

MovablePlatform *createPlatform(Vec2 position, int width, PlatformType type);

void platformMove(MovablePlatform *platform);

Entity platformGetBody(MovablePlatform const *platform, int index);

Hitbox *platformGetHitbox(MovablePlatform const *platform, int index);

Vec2 platformGetVelocity(MovablePlatform const *platform);

int platformGetSize(MovablePlatform const *platform);

void destroyPlatform(MovablePlatform *platform);