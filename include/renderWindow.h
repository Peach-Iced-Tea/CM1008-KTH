#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.h"

// Keep GLOBAL_SCALER to a power of 2 (i.e. 1, 2, 4, 8...) for the most optimal scaling.
#define GLOBAL_SCALER 2

typedef struct renderWindow RenderWindow;

RenderWindow *createRenderWindow(const char* pTitle, int w, int h);

/*
Load a new SDL_Texture* using the pRenderer of the given RenderWindow.

Accepts a relative file path or an absolute file path.
*/
SDL_Texture *loadTexture(RenderWindow *pRenderWindow, const char* pFilePath);

/*
Clear the previous frame off of the given RenderWindow.

Use this in preparation for the next displayWindow().
*/
void clearWindow(RenderWindow *pRenderWindow);

/*
Renders the given Entity on the screen if it is visible inside the camera dimensions.
*/
void renderEntity(RenderWindow *pRenderWindow, Entity *pEntity);

/*
Display the current latest frame inside the buffer of the given RenderWindow.
*/
void displayWindow(RenderWindow *pRenderWindow);

/*
Use this function to destroy the given RenderWindow pointer and free up memory.
*/
void destroyRenderWindow(RenderWindow *pRenderWindow);


//Fixa får inte ligga här
void drawLine(RenderWindow *pRenderWindow, Vec2 pos1, Vec2 pos2);