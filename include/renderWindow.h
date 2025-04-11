#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.h"
// Keep GLOBAL_SCALER to a power of 2 (i.e. 1, 2, 4, 8...) for the most optimal scaling.
#define GLOBAL_SCALER 2

typedef struct renderWindow RenderWindow;

RenderWindow *createRenderWindow(const char* p_title, int p_w, int p_h);

SDL_Texture *loadTexture(const char* p_filePath, RenderWindow *pRenderWindow);

void cleanUp(RenderWindow *pRenderWindow);

void clearWindow(RenderWindow *pRenderWindow);
void renderEntity(RenderWindow *pRenderWindow, Entity *p_entity);
void displayWindow(RenderWindow *pRenderWindow);
