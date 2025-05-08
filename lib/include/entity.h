#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdbool.h>

#include "vmath.h"

#define ENTITY_PLAYER 1
#define ENTITY_TONGUE 2
#define ENTITY_CROSSHAIR 3

#define HITBOX_NONE 0
#define HITBOX_PLAYER 1 // Create a hitbox specifically designated for a player Entity.
#define HITBOX_FULL_BLOCK 2 // Create a hitbox that is exactly the same size as the sprite/image.
#define HITBOX_HALF_BLOCK 3 // Create a hitbox that is half the height of the sprite/image height.

#define OBJECT_IS_NORTH 1   // The object is above the reference.
#define OBJECT_IS_SOUTH 2   // The object is below the reference.
#define OBJECT_IS_WEST 3    // The object is to the left of the reference.
#define OBJECT_IS_EAST 4    // The object is to the right of the reference.

#define GRAVITY_ACCELERATION 60.0f    // The simulation of gravity uses this constant.
#define MAX_GRAVITY_VELOCITY 660.0f // The absolute max value that the velocity of gravity can reach.

typedef struct hitbox Hitbox;

typedef struct entity {
    SDL_FRect frame;
    SDL_Rect source;
    Hitbox *pHitbox;
} Entity;

/*
Create a hitbox based on the type specified with hitboxType.
*/
Hitbox *createHitbox(Vec2 const position, float w, float h, int hitboxType);

/*

*/
int entityInitData(Entity *pEntity, Vec2 position, int entityType, int hitboxType);

/*
Correct the position of the given Entity in case of a collision with another object.
Expects a Vec2 containing the correctional values for the x-axis and y-axis.
*/
void entityCollisionResponse(Entity *pEntity, Vec2 const correction);

/*

*/
void entitySetPosition(Entity *pEntity, Vec2 const vector);

/*

*/
void entitySetSource(Entity *pEntity, SDL_Rect newSource);

/*

*/
void entityMove(Entity *pEntity, Vec2 velocity);

/*
Get the current position on both the x-axis and y-axis for the given Entity.
*/
Vec2 entityGetPosition(Entity const entity);

/* 
Get the current midpoint of the entity struct (midpoint of the sprite).
*/
Vec2 entityGetMidPoint(Entity const entity);

/*
Call this function to update the positional values of a hitbox.
Adds the given vector to the current positional values of x and y.
*/
void hitboxPositionAdd(Hitbox *pHitbox, Vec2 const vector);

/*
Call this function to update the positional values of a hitbox.
Subtracts the given vector from the current positional values of x and y.
*/
void hitboxPositionSub(Hitbox *pHitbox, Vec2 const vector);

/*
Check if there is any collision between two hitboxes.

Returns 'true' if collision has been detected, otherwise returns 'false'.
*/
bool hitboxCheckCollision(Hitbox const *pObject1, Hitbox const *pObject2);

/*
Returns the current orientation of pObject relative to pReference.
*/
int hitboxOrientation(Hitbox const *pObject, Hitbox const *pReference);

/*
Compare two rectangles to see how they have collided.

Returns a Vec2 containing the response to correct the collision.
*/
Vec2 rectVsRect(Hitbox const *pObject, Hitbox const *pReference);

/*
Check if a ray has any intersection along its trajectory.

Returns a float containing the amount of "time" until collision.
*/
float rayVsRect(Hitbox const *pObject, Hitbox const *pReference);

/*
Check if a point in space (x,y) is inside the given Hitbox.

Returns 'true' if the point is inside the given Hitbox, otherwise returns 'false'.
*/
bool pointVsRect(Hitbox const *pHitbox, Vec2 const point);

/*
Get the position of the given Hitbox.

Returns a Vec2 containing the x and y coordinates.
*/
Vec2 getHitboxPosition(Hitbox const *pHitbox);

/*
Get the halfSize of the given Hitbox.

Returns a Vec2 containing the width (.x) and height (.y).
*/
Vec2 getHitboxHalfSize(Hitbox const *pHitbox);

/*
Use this function to destroy the given Hitbox pointer and free up memory.
*/
void destroyHitbox(Hitbox *pHitbox);
