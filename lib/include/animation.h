#pragma once

#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>

#include "vmath.h"

bool animationUpdateRotation(SDL_Rect *pAnimation, float angle);

void animationUpdateRunning(SDL_Rect *pAnimation, int *pTimer);

