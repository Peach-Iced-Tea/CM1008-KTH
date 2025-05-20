#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#include "entity.h"
#include "vmath.h"
#include "camera.h"

typedef enum RenderType {
    RENDER_PLAYER1, RENDER_PLAYER2, RENDER_TONGUE,
    RENDER_OBSTACLE, RENDER_MOUSE, RENDER_PLATFORM,
    RENDER_MAP
} RenderType;

typedef struct renderWindow RenderWindow;

RenderWindow *createRenderWindow(char const *title, int w, int h);

/*
Load a map tileset from the given file path.
*/
int windowLoadMapTileset(RenderWindow *renderWindow, char const *filePath);

/*
Handles key inputs related to the functions of the RenderWindow data type.

Such as toggling fullscreen on and off.
*/
void windowHandleInput(RenderWindow *renderWindow, Input const *input);

/*
Renders a Hitbox on the screen if renderHitboxes has been set to 'true'.

The Hitbox will be drawn with a yellow perimeter.
*/
void windowRenderHitbox(RenderWindow *renderWindow, Hitbox const *hitbox);

/*
Renders a menu with a specified amount of buttons, expects the position and source to be given for each button.
*/
void windowRenderMenu(RenderWindow *renderWindow, SDL_Texture *texture, SDL_Rect menuButtons[], SDL_Rect menuPosition[], int nrOfButtons);

/*
Renders the given text on the screen in the color white.

The position given with x and y represents the middle point of the text.
*/
void windowRenderText(RenderWindow *renderWindow, char const textToRender[], int x, int y);

/*
Renders the given Entity on the screen if it is visible inside the camera dimensions.
*/
void windowRenderEntity(RenderWindow *renderWindow, Entity const entity, RenderType renderType);

/*
Clear the previous frame off of the given RenderWindow.

Use this in preparation for the next displayWindow().
*/
void windowClearFrame(RenderWindow *renderWindow);

/*
Display the current latest frame inside the buffer of the given RenderWindow.
*/
void windowDisplayFrame(RenderWindow *renderWindow);

/*
Draws a line on the screen that starts at pos1 and ends at pos2.
*/
void windowDrawLine(RenderWindow *renderWindow, Vec2 pos1, Vec2 pos2);

/*
Set the Camera object that the RenderWindow should use when rendering objects on the screen.
*/
void windowSetCamera(RenderWindow *window, Camera *camera);

/*
Get the SDL_Renderer* from the given RenderWindow.
*/
SDL_Renderer *windowGetRenderer(RenderWindow const *renderWindow);

/*
Get the full window width of the given RenderWindow.
*/
int windowGetWidth(RenderWindow const *renderWindow);

/*
Get the full window height of the given RenderWindow.
*/
int windowGetHeight(RenderWindow const *renderWindow);

/*
Use this function to destroy the given RenderWindow pointer and free up memory.
*/
void destroyRenderWindow(RenderWindow *renderWindow);
