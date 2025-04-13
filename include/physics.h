#pragma once
#include <math.h>
#include <stdbool.h>

#include "vmath.h"

#define HITBOX_NONE 0
#define HITBOX_PLAYER 1 // Create a hitbox specifically designated for a player Entity.
#define HITBOX_FULL_BLOCK 2 // Create a hitbox that is exactly the same size as the sprite/image.
#define HITBOX_HALF_BLOCK 3 // Create a hitbox that is half the height of the sprite/image height.

#define MAX_PLAYER_VELOCITY 380.0f  // The absolute max value of the velocity a player can reach.
#define MAX_GRAVITY_VELOCITY 660.0f // The absolute max value that the velocity of gravity can reach.

#define PLAYER_VELOCITY 60.0f   // Initial velocity to be applied on a player upon moving.
#define PLAYER_ACCELERATION 580.0f  // A constant for the acceleration of a player, this value gets modified by deltaTime before updating velocity.
#define JUMP_VELOCITY 550.0f    // The velocity to be applied when a player presses jump, add negative sign before this value.
#define GRAVITY_ACCELERATION 1960.0f    // The simulation of gravity uses this constant.

typedef struct hitbox Hitbox;

// Add a new function that consolidates all the Hitbox types into one, that then calls the other specific functions based on hitboxType in-parameter.

Hitbox *createPlayerHitbox(Vec2 const position, float w, float h);
Hitbox *createFullBlockHitbox(Vec2 const position, float w, float h);
Hitbox *createHalfBlockHitbox(Vec2 const position, float w, float h);

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
Check if there is any collision between two hitboxes.

Returns 'true' if collision has been detected.
Returns 'false' if no collision has been detected.
*/
bool checkCollision(Hitbox const *pObject1, Hitbox const *pObject2);

// Change this function to have it return the relative position of Object1 compared to Object2.
bool hitboxIsAbove(Hitbox const *pObject, Hitbox const *pReference);

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
Use this function to destroy the given Hitbox pointer and free up memory.
*/
void destroyHitbox(Hitbox *pHitbox);