#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#include "vmath.h"
#include "physics.h"

#define ENTITY_PLAYER 1
#define ENTITY_TONGUE 2

typedef struct entity Entity;

Entity *createEntity(Vec2 position, SDL_Texture *pTexture, int entityType, int hitboxType);

/*
Returns the SDL_Texture* of the given Entity.
*/
SDL_Texture *entityGetTexture(Entity const *pEntity);

/*
Returns the SDL_FRect of the given Entity.
*/
SDL_FRect entityGetCurrentFrame(Entity const *pEntity);

/*
Correct the position of the given Entity in case of a collision with another object.
Expects a Vec2 containing the correctional values for the x-axis and y-axis.
*/
void entityCollisionResponse(Entity *pEntity, Vec2 const correction);


void entitySetPosition(Entity *pEntity, Vec2 const vector);


void entityMove(Entity *pEntity, Vec2 velocity);

/*
Get the current position on both the x-axis and y-axis for the given Entity.
*/
Vec2 entityGetPosition(Entity const *pEntity);

/* 
Get the current midpoint of the entity struct (midpoint of the sprite).
*/
Vec2 entityGetMidPoint(Entity const *pEntity);

/*
Returns the Hitbox of the given Entity.
*/
Hitbox *entityGetHitbox(Entity const *pEntity);

/*
Use this function to destroy the given Entity pointer and free up memory.
*/
void destroyEntity(Entity *pEntity);