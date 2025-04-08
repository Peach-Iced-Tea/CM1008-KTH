#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Entity.h"

#include "Vmath.h"

struct entity{
    Vector2f pos;
    //float x, y;
    SDL_Rect currentFrame;
    SDL_Texture* tex;
};


Entity *createEntiy(Vector2f p_pos, SDL_Texture *p_tex)
{
    Entity *pEntity = malloc(sizeof(Entity));
    //pEntity->x = p_x;
    //pEntity->y = p_y;
    pEntity->pos = p_pos;

    pEntity->tex = p_tex;

    pEntity->currentFrame.x = 0;
    pEntity->currentFrame.y = 0;
    pEntity->currentFrame.w = 32;
    pEntity->currentFrame.h = 32;
    return pEntity;
}
/*
//Get X of entity
 float getX(Entity *pEntity)
{
    return pEntity->x;
}
//Get Y of entity
float getY(Entity *pEntity)
{
    return pEntity->y;
} */

Vector2f getPos(Entity *pEntity)
{
    //Vector2f *vector = malloc(sizeof(Vector2f*));
    return pEntity->pos;
}


//returns texture of given Entity
SDL_Texture *getTex(Entity *pEntity)
{
    return pEntity->tex;
}
//returns currentFrame of given Entity
SDL_Rect getCurrentFrame(Entity *pEntity)
{
    return  pEntity->currentFrame;
}


void movePlayer(Entity *pEntity, float x, float y)
{
    //printf("x: %f || y: %f\n", x, y);
    //pEntity->pos.vx = x;
    //pEntity->pos.vy = y;
    pEntity->pos.y +=y;
    pEntity->pos.x +=x;
}


void updateEntity(Entity *pEntity)
{
    //printf("x: %f || y: %f\n", pEntity->pos.vx, pEntity->pos.vy);
    pEntity->pos.y +=pEntity->pos.vy;
    pEntity->pos.x +=pEntity->pos.vx;
    //pEntity->pos.vx = 0;
    //pEntity->pos.vy = 0;
}