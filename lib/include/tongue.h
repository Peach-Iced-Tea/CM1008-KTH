#pragma once
#include <stdbool.h>

#include "entity.h"
#include "vmath.h"

typedef enum TongueState {
    NEUTRAL, EXTENDING, RETRACTING, HIT_ENTITY
} TongueState;

typedef struct {
    Vec2 position;
    Vec2 velocity;
    Vec2 mousePosition;
    TongueState state;
    float length;
} TongueInfo;

typedef struct tongue Tongue;

Tongue *createTongue(Vec2 position);

/*
Determines whether the velocity of the given Tongue should be going towards mousePosition,
or if it should be going back towards the centerPoint.
*/
void tongueUpdateVelocity(Tongue *tongue, Vec2 centerPoint);

/*
Calling this function will move the Tongue if it currently has a velocity.
*/
void tongueUpdatePosition(Tongue *tongue, Vec2 centerPoint, float timestep);

/*
Calculates how the tongue shaft should be angled,
as well as how long the shaft should be and where the middle point of the shaft is.
*/
void tongueCalculateShaft(Tongue *tongue, Vec2 centerPoint, Vec2 referencePoint);

/*
This function checks whether the given Tongue has collided with the given Entity.

If a collision occurs then the Tongue will move to the middle point of the Entity.
*/
bool tongueCheckCollision(Tongue *tongue, Entity const entity);

/*
Sets the mouse position of the given Tongue.

Mouse position is considered to be the aim for the tongue.
*/
bool tongueSetMousePosition(Tongue *tongue, Vec2 mousePosition);

/*
Sets the position of the given Tongue.
*/
void tongueSetPosition(Tongue *tongue, Vec2 newPosition);

/*
Returns a struct containing all relevant information about the given Tongue.
*/
TongueInfo tongueGetInfo(Tongue const *tongue);

/*
Get the tip (Entity) of the given Tongue.
*/
Entity tongueGetTip(Tongue const *tongue);

/*
Get the shaft (Entity) of the given Tongue.
*/
Entity tongueGetShaft(Tongue const *tongue);

/*
Get the Hitbox of the tip (Entity) of the Given Tongue.
*/
Hitbox *tongueGetTipHitbox(Tongue const *tongue);

/*
This function is very aggressive and overrides the velocity of the given Tongue.

!!! Should only be used if you're completely sure.
*/
void tongueOverrideVelocity(Tongue *tongue, Vec2 velocity);

/*
Use this function to destroy the given Tongue pointer and free up memory.
*/
void destroyTongue(Tongue *tongue);