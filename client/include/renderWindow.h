#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#include "entity.h"
#include "camera.h"
#include "player.h"
#include "mapParser.h"
#include "mouse.h"


typedef struct renderWindow RenderWindow;

RenderWindow *createRenderWindow(char const *pTitle, int w, int h);

/*
Handles key inputs related to the functions of the RenderWindow data type.

Such as toggling fullscreen on and off.
*/
void windowHandleInput(RenderWindow *pRenderWindow, Input const *pInputs);

/*
Load a new SDL_Texture* using the pRenderer of the given RenderWindow.

Accepts a relative file path or an absolute file path.
*/
SDL_Texture *windowLoadTexture(RenderWindow *pRenderWindow, char const *pFilePath);

/*

*/
void windowRenderHitbox(RenderWindow *pRenderWindow, Hitbox const *pHitbox, Camera const *pCamera);

/*

*/
void windowRenderMenu(RenderWindow *pRenderWindow, SDL_Texture *pTexture, SDL_Rect menuButtons[], SDL_Rect menuPosition[], int nrOfButtons);

/*

*/
void windowRenderText(RenderWindow *pRenderWindow, char const textToRender[], int x, int y);

/*
Renders the given Entity on the screen if it is visible inside the camera dimensions.
*/
void windowRenderEntity(RenderWindow *pRenderWindow, Entity const *pEntity, Camera const *pCamera);

/*
Renders the given Player on the screen if it is visible inside the camera dimensions.
*/
void windowRenderPlayer(RenderWindow *pRenderWindow, Player const *pPlayer, Camera const *pCamera);

/*

*/
void windowRenderMapLayer(RenderWindow *pRenderWindow, ClientMap *pMap, Camera const *pCamera);

/*

*/
void windowRenderCrosshair(RenderWindow *pRenderWindow, Crosshair const *pCrosshair, Camera *pCamera);

/*
Clear the previous frame off of the given RenderWindow.

Use this in preparation for the next displayWindow().
*/
void windowClearFrame(RenderWindow *pRenderWindow);

/*
Display the current latest frame inside the buffer of the given RenderWindow.
*/
void windowDisplayFrame(RenderWindow *pRenderWindow);


void windowDrawLine(RenderWindow *pRenderWindow, Vec2 pos1, Vec2 pos2, Camera const *pCamera);

/*
Get the SDL_Renderer* from the given RenderWindow.
*/
SDL_Renderer *windowGetRenderer(RenderWindow const *pRenderWindow);

/*

*/
int windowGetWidth(RenderWindow const *pRenderWindow);

/*

*/
int windowGetHeight(RenderWindow const *pRenderWindow);

/*
Use this function to destroy the given RenderWindow pointer and free up memory.
*/
void destroyRenderWindow(RenderWindow *pRenderWindow);
