#pragma once
#include "entity.h"
#include "input.h"

typedef struct crosshair Crosshair;

Crosshair *createCrosshair(Vec2 const position);

/*
Handles the inputs related to the Crosshair.
*/
void crosshairHandleInput(Crosshair *crosshair, Input *input);

/*
Calling this function updates the current position of the Crosshair based on the referencePosition.
*/
void crosshairUpdatePosition(Crosshair *crosshair, Vec2 referencePosition);

/*
Set the borders that the crosshair can move within relative to its referencePosition.
*/
void crosshairSetBorders(Crosshair *pCrosshair, float borderX, float borderY);

/*
Get the body (Entity) of the given Crosshair.
*/
Entity crosshairGetBody(Crosshair const *crosshair);

/*
Get the positional values of the given Crosshair.

The returned values of the position are the middle point of the Crosshair.
*/
Vec2 crosshairGetPosition(Crosshair const *crosshair);

/*
Use this function to destroy the given Crosshair pointer and free up memory.
*/
void destroyCrosshair(Crosshair *crosshair);