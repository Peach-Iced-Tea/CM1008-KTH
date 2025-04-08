#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct entity Entity;


Entity *createEntiy(float p_x, float p_y, SDL_Texture *p_tex);

float getX(Entity *pEntity);
float getY(Entity *pEntity);

SDL_Texture *getTex(Entity *pEntity);
SDL_Rect getCurrentFrame(Entity *pEntity);
