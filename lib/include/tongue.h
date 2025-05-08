#pragma once
#include <stdbool.h>

#include "entity.h"
#include "vmath.h"

typedef enum {
    NEUTRAL, EXTENDING, RETRACTING, HIT_ENTITY
} TongueState;

typedef struct {
    Vec2 position;
    Vec2 velocity;
    Vec2 mousePosition;
    float length;
    float angle;
    TongueState state;
    SDL_FRect tongueShaft;
} TongueInfo;

typedef struct tongue Tongue;

Tongue *createTongue(Vec2 position);

void tongueUpdateVelocity(Tongue *tongue, Vec2 centerPoint);

void tongueUpdatePosition(Tongue *tongue, Vec2 centerPoint, float timestep);

void tongueCalculateShaft(Tongue *tongue, Vec2 centerPoint, Vec2 referencePoint);

bool tongueCheckCollision(Tongue *tongue, Entity const entity);

bool tongueSetMousePosition(Tongue *tongue, Vec2 mousePosition);

void tongueSetPosition(Tongue *tongue, Vec2 newPosition);

void tongueSetShaftMidPoint(Tongue *tongue, Vec2 newPosition);

TongueInfo tongueGetInfo(Tongue const *tongue);

Entity tongueGetTip(Tongue const *tongue);

Hitbox *tongueGetHitbox(Tongue const *tongue);

void tongueOverrideVelocity(Tongue *tongue, Vec2 velocity);

void destroyTongue(Tongue *tongue);