#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#include "entity.h"
#include "camera.h"
#include "mapParser.h"
#include "tongue.h"

typedef enum RenderType {
    RENDER_PLAYER1, RENDER_PLAYER2, RENDER_TONGUE,
    RENDER_OBSTACLE, RENDER_CROSSHAIR, RENDER_CHECKPOINT
} RenderType;

typedef struct renderWindow RenderWindow;

RenderWindow *createRenderWindow(char const *title, int w, int h);

/*
Handles key inputs related to the functions of the RenderWindow data type.

Such as toggling fullscreen on and off.
*/
void windowHandleInput(RenderWindow *renderWindow, Input const *input);

/*

*/
void windowRenderHitbox(RenderWindow *renderWindow, Hitbox const *hitbox);

/*

*/
void windowRenderMenu(RenderWindow *renderWindow, SDL_Texture *texture, SDL_Rect menuButtons[], SDL_Rect menuPosition[], int nrOfButtons);

/*

*/
void windowRenderText(RenderWindow *renderWindow, char const textToRender[], int x, int y);

/*
Renders the given Entity on the screen if it is visible inside the camera dimensions.
*/
void windowRenderObject(RenderWindow *renderWindow, Entity const entity, RenderType renderType);

/*

*/
void windowRenderMapLayer(RenderWindow *renderWindow, ClientMap *map);

/*
Clear the previous frame off of the given RenderWindow.

Use this in preparation for the next displayWindow().
*/
void windowClearFrame(RenderWindow *renderWindow);

/*
Display the current latest frame inside the buffer of the given RenderWindow.
*/
void windowDisplayFrame(RenderWindow *renderWindow);


void windowDrawLine(RenderWindow *renderWindow, Vec2 pos1, Vec2 pos2);

/*

*/
void windowSetCamera(RenderWindow *window, Camera *camera);

/*
Get the SDL_Renderer* from the given RenderWindow.
*/
SDL_Renderer *windowGetRenderer(RenderWindow const *renderWindow);

/*

*/
int windowGetWidth(RenderWindow const *renderWindow);

/*

*/
int windowGetHeight(RenderWindow const *renderWindow);

/*
Use this function to destroy the given RenderWindow pointer and free up memory.
*/
void destroyRenderWindow(RenderWindow *renderWindow);
