#pragma once
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "entity.h"
#include "vmath.h"

typedef enum {
    NEUTRAL, EXTENDING, RETRACTING, MAX_EXTENSION
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

Tongue *createTongue(Vec2 position, SDL_Renderer *pRenderer);

bool tongueSetMousePosition(Tongue *pTongue, Vec2 mousePosition);

void tongueSetPosition(Tongue *pTongue, Vec2 newPosition);

void tongueSetShaftMidPoint(Tongue *pTongue, Vec2 newPosition);

void tongueSetVelocity(Tongue *pTongue, Vec2 centerPoint);

void tongueUpdate(Tongue *pTongue, Vec2 centerPoint, float timestep);

void tongueCalculateShaft(Tongue *pTongue, Vec2 centerPoint, Vec2 referencePoint);

bool tongueCheckCollision(Tongue *pTongue, Entity *pEntity);

TongueInfo tongueGetInfo(Tongue const *pTongue);

Entity *tongueGetTip(Tongue const *pTongue);

Hitbox *tongueGetHitbox(Tongue const *pTongue);

SDL_Texture *tongueGetShaftTexture(Tongue const *pTongue);

void destroyTongue(Tongue *pTongue);