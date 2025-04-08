#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


#include "Vmath.h"

typedef struct entity Entity;


Entity *createEntiy(Vector2f p_pos, SDL_Texture *p_tex);

Vector2f getPos(Entity *pEntity);
//float getX(Entity *pEntity);
//float getY(Entity *pEntity);

SDL_Texture *getTex(Entity *pEntity);
SDL_Rect getCurrentFrame(Entity *pEntity);




void movePlayer(Entity *pEntity, float x, float y);
//void movement(Entity *pEntity);
void updateEntity(Entity *pEntity);