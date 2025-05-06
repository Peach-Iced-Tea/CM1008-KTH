#pragma once
#include "entity.h"
#include "input.h"

typedef struct crosshair Crosshair;

Crosshair *createCrosshair(SDL_Renderer *renderer, Vec2 const position);

void crosshairHandleInput(Crosshair *crosshair, Input *input);

void crosshairUpdatePosition(Crosshair *crosshair, Vec2 referencePosition);

Entity *crosshairGetBody(Crosshair const *pCrosshair);

Vec2 crosshairGetPosition(Crosshair const *pCrosshair);

SDL_Rect crosshairGetSheetPosition(Crosshair const *pCrosshair);

void destroyCrosshair(Crosshair *crosshair);