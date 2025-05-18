#pragma once
#include "entity.h"
#include "input.h"

typedef struct mouse Mouse;

Mouse *createMouse(Vec2 const position);

/*
Handles the inputs related to the Mouse.
*/
void mouseHandleInput(Mouse *Mouse, Input *input);

/*
Calling this function updates the current position of the Mouse based on the referencePosition.
*/
void mouseUpdatePosition(Mouse *Mouse, Vec2 referencePosition);

/*
Set the borders that the Mouse can move within relative to its referencePosition.
*/
void mouseSetBorders(Mouse *pMouse, float borderX, float borderY);

/*
Get the body (Entity) of the given Mouse.
*/
Entity mouseGetBody(Mouse const *Mouse);

/*
Get the positional values of the given Mouse.

The returned values of the position are the middle point of the Mouse.
*/
Vec2 mouseGetPosition(Mouse const *Mouse);

/*
Use this function to destroy the given Mouse pointer and free up memory.
*/
void destroyMouse(Mouse *Mouse);