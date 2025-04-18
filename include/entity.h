#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#include "vmath.h"
#include "physics.h"

typedef struct entity Entity;

Entity *createEntity(Vec2 position, SDL_Texture *pTexture, int hitboxType);

/*
Returns the SDL_Texture* of the given Entity.
*/
SDL_Texture *getTexture(Entity const *pEntity);

/*
Returns the SDL_FRect of the given Entity.
*/
SDL_FRect getCurrentFrame(Entity const *pEntity);

/*
Correct the position of the given Entity in case of a collision with another object.
Expects a Vec2 containing the correctional values for the x-axis and y-axis.
*/
void collisionResponse(Entity *pEntity, Vec2 const correction);

/* 
Set the base velocity(x-axis) of the given Entity, gets updated on every call to updateVelocity().
Expects a velocity for the x-axis, 0.0f means no velocity in the x-axis.
*/
bool setVelocityX(Entity *pEntity, float v_x);

/* 
Set the base velocity(y-axis) of the given Entity, gets updated on every call to updateVelocity().
Expects a velocity for the y-axis, 0.0f means no velocity in the y-axis.
*/
bool setVelocityY(Entity *pEntity, float v_y);

/* 
Set the acceleration that should be applied to the velocity(x-axis) on every updateVelocity().
This value does not change after it is set.
Expects an acceleration for the x-axis.
*/
bool setAccelerationX(Entity *pEntity, float a_x);

/* 
Set the acceleration that should be applied to the velocity(y-axis) on every updateVelocity().
This value does not change after it is set.
Expects an acceleration for the y-axis.
*/
bool setAccelerationY(Entity *pEntity, float a_y);

/*
Updates the current x and y coordinates of the given Entity.
The values of velocity(x,y) gets added to the current position(x,y).
Use "deltaTime" to modulate how much the Entity moves on every updatePosition().
*/
void updatePosition(Entity *pEntity, float deltaTime);

/*
Updates the current x and y velocity values of the given Entity.
The values of acceleration(x,y) gets added to the current velocity(x,y).
Use "deltaTime" to modulate how much acceleration is applied on each updateVelocity().
*/
void updateVelocity(Entity *pEntity, float deltaTime);

/*
Get the current position on both the x-axis and y-axis for the given Entity.
*/
Vec2 getPosition(Entity const *pEntity);

/*
Get the current velocity on both the x-axis and y-axis for the given Entity.
*/
Vec2 getVelocity(Entity const *pEntity);

/*
Get the current acceleration on both the x-axis and y-axis for the given Entity.
*/
Vec2 getAcceleration(Entity const *pEntity);

/* 
Get the current midpoint of the entity struct (midpoint of the sprite).
*/
Vec2 getMidPoint(Entity const *pEntity);

/*
Returns the Hitbox of the given Entity.
*/
Hitbox *getHitbox(Entity const *pEntity);

/*
Use this function to destroy the given Entity pointer and free up memory.
*/
void destroyEntity(Entity *pEntity);