#pragma once
#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>

#include "vmath.h"

typedef enum EntityType {
    ENTITY_DEFAULT, ENTITY_PLAYER, ENTITY_TONGUE_TIP, ENTITY_TONGUE_SHAFT, ENTITY_CROSSHAIR, ENTITY_END
} EntityType;

typedef enum HitboxType {
    HITBOX_NONE, HITBOX_PLAYER, HITBOX_FULL_BLOCK,
    HITBOX_HALF_BLOCK_TOP, HITBOX_HALF_BLOCK_BOTTOM, HITBOX_HALF_BLOCK_LEFT, HITBOX_HALF_BLOCK_RIGHT,
    HITBOX_END
} HitboxType;

#define OBJECT_IS_NORTH 1   // The object is above the reference.
#define OBJECT_IS_SOUTH 2   // The object is below the reference.
#define OBJECT_IS_WEST 3    // The object is to the left of the reference.
#define OBJECT_IS_EAST 4    // The object is to the right of the reference.

#define GRAVITY_ACCELERATION 60.0f      // The simulation of gravity uses this constant.
#define MAX_GRAVITY_VELOCITY 660.0f     // The absolute max value that the velocity of gravity can reach.

typedef struct hitbox Hitbox;

typedef struct entity {
    SDL_FRect frame;
    SDL_Rect source;
    Hitbox *pHitbox;
    SDL_RendererFlip flip;
    float angle;
    EntityType type;
} Entity;

/*
Create a hitbox based on the type specified with hitboxType.
*/
Hitbox *createHitbox(Vec2 const position, float w, float h, int hitboxType);

/*
Initialise all the values of the given Entity based on the entityType that is given.
*/
int entityInitData(Entity *entity, Vec2 position, int entityType, int hitboxType);

/*
Correct the position of the given Entity in case of a collision with another object.
Expects a Vec2 containing the correctional values for the x-axis and y-axis.
*/
void entityCollisionResponse(Entity *entity, Vec2 const correction);

/*
Moves the given Entity from its current position based on the values inside velocity.
*/
void entityMove(Entity *entity, Vec2 velocity);

/*
This function overrides the current position of the given Entity with the supplied values inside newPosition.
*/
void entitySetPosition(Entity *entity, Vec2 const newPosition);

/*
Sets the values inside source paramater with the values of newSource.
*/
void entitySetSource(Entity *entity, SDL_Rect newSource);

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
void hitboxPositionAdd(Hitbox *hitbox, Vec2 const vector);

/*
Call this function to update the positional values of a hitbox.
Subtracts the given vector from the current positional values of x and y.
*/
void hitboxPositionSub(Hitbox *hitbox, Vec2 const vector);

/*
Check if there is any collision between two hitboxes.

Returns 'true' if collision has been detected, otherwise returns 'false'.
*/
bool hitboxCheckCollision(Hitbox const *object1, Hitbox const *object2);

/*
Returns the current orientation of pObject relative to pReference.
*/
int hitboxOrientation(Hitbox const *object, Hitbox const *reference);

/*
Compare two rectangles to see how they have collided.

Returns a Vec2 containing the response to correct the collision.
*/
Vec2 rectVsRect(Hitbox const *object, Hitbox const *reference);

/*
Check if a ray has any intersection along its trajectory.

Returns a float containing the amount of "time" until collision.
*/
float rayVsRect(Hitbox const *object, Hitbox const *reference);

/*
Check if a point in space (x,y) is inside the given Hitbox.

Returns 'true' if the point is inside the given Hitbox, otherwise returns 'false'.
*/
bool pointVsRect(Hitbox const *hitbox, Vec2 const point);

/*
Get the position of the given Hitbox.

Returns a Vec2 containing the x and y coordinates.
*/
Vec2 hitboxGetPosition(Hitbox const *hitbox);

/*
Get the halfSize of the given Hitbox.

Returns a Vec2 containing the width (.x) and height (.y).
*/
Vec2 hitboxGetHalfSize(Hitbox const *hitbox);

/*
Use this function to destroy the given Hitbox pointer and free up memory.
*/
void destroyHitbox(Hitbox *hitbox);
