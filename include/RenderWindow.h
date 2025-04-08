#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Entity.h"

typedef struct renderWindow RenderWindow;


RenderWindow *create_renderWindow(const char* p_title, int p_w, int p_h);

SDL_Texture *loadTexture(const char* p_filePath, RenderWindow *pRenderWindow);

void cleanUp(RenderWindow *pRenderWindow);

void clearWindow(RenderWindow *pRenderWindow);
void render(RenderWindow *pRenderWindow, Entity *p_entity );
void display(RenderWindow *pRenderWindow);


