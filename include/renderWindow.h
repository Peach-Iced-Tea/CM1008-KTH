#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#include "entity.h"
#include "camera.h"
#include "player.h"

typedef struct renderWindow RenderWindow;

RenderWindow *createRenderWindow(const char* pTitle, int w, int h);

/*
This function will set the game window of the given RenderWindow to fullscreen,
unless it is already in fullscreen, then it will set it to Borderless instead.
*/
void toggleFullscreen(RenderWindow *pRenderWindow);

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
void renderEntity(RenderWindow *pRenderWindow, Entity *pEntity, Camera *pCamera);

/*
Renders the given Player on the screen if it is visible inside the camera dimensions.
*/
void renderPlayer(RenderWindow *pRenderWindow, Player *pPlayer, Camera *pCamera);

/*
Display the current latest frame inside the buffer of the given RenderWindow.
*/
void displayWindow(RenderWindow *pRenderWindow);


void drawLine(RenderWindow *pRenderWindow, Vec2 pos1, Vec2 pos2, Camera *pCamera);

/*
Get the SDL_Renderer* from the given RenderWindow.
*/
SDL_Renderer *getRenderer(RenderWindow *pRenderWindow);

/*
Use this function to destroy the given RenderWindow pointer and free up memory.
*/
void destroyRenderWindow(RenderWindow *pRenderWindow);