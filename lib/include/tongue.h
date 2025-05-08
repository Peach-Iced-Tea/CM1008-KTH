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

bool tongueSetMousePosition(Tongue *pTongue, Vec2 mousePosition);

void tongueSetPosition(Tongue *pTongue, Vec2 newPosition);

void tongueSetShaftMidPoint(Tongue *pTongue, Vec2 newPosition);

void tongueSetVelocity(Tongue *pTongue, Vec2 centerPoint);

void tongueUpdate(Tongue *pTongue, Vec2 centerPoint, float timestep);

void tongueCalculateShaft(Tongue *pTongue, Vec2 centerPoint, Vec2 referencePoint);

bool tongueCheckCollision(Tongue *pTongue, Entity const entity);

TongueInfo tongueGetInfo(Tongue const *pTongue);

Entity tongueGetTip(Tongue const *pTongue);

Hitbox *tongueGetHitbox(Tongue const *pTongue);

void tongueOverrideVelocity(Tongue *pTongue, Vec2 velocity);

void destroyTongue(Tongue *pTongue);