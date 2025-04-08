#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#include "Vmath.h"

#define HITBOX_PLAYER 1
#define HITBOX_32BLOCK 2

#define BLOCK_UP 1
#define BLOCK_DOWN 2
#define BLOCK_LEFT 1
#define BLOCK_RIGHT 2

struct hitbox {
    float x, y, w, h;
};
typedef struct hitbox Hitbox;
typedef struct entity Entity;


Entity *createEntiy(Vector2f p_pos, SDL_Texture *p_tex, int hitboxType);

void createPlayerHitbox(Hitbox *pBox, Vector2f *pPos, float w, float h);
void create32BlockHitbox(Hitbox *pBox, Vector2f *pPos, float w, float h);


Hitbox getHPos(Entity *pEntity);

Vector2f getPos(Entity *pEntity);
//float getX(Entity *pEntity);
//float getY(Entity *pEntity);

SDL_Texture *getTex(Entity *pEntity);
SDL_Rect getCurrentFrame(Entity *pEntity);




void movePlayer(Entity *pEntity, float x, float y);
//void movement(Entity *pEntity);
void updateEntity(Entity *pEntity);
int checkIntersection(Entity *pPlayer, Entity *pEntity, float directionX, float directionY);

void whatta();