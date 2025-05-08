#pragma once
#include "entity.h"
#include "input.h"

typedef struct crosshair Crosshair;

Crosshair *createCrosshair(Vec2 const position);

void crosshairHandleInput(Crosshair *crosshair, Input *input);

void crosshairUpdatePosition(Crosshair *crosshair, Vec2 referencePosition);

Entity crosshairGetBody(Crosshair const *crosshair);

Vec2 crosshairGetPosition(Crosshair const *crosshair);

SDL_Rect crosshairGetSheetPosition(Crosshair const *crosshair);

void destroyCrosshair(Crosshair *crosshair);