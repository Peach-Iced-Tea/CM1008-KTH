#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Entity.h"

struct entity{
    float x, y;
    SDL_Rect currentFrame;
    SDL_Texture* tex;
};


Entity *createEntiy(float p_x, float p_y, SDL_Texture *p_tex)
{
    Entity *pEntity = malloc(sizeof(Entity));
    pEntity->x = p_x;
    pEntity->y = p_y;
    pEntity->tex = p_tex;

    pEntity->currentFrame.x = 0;
    pEntity->currentFrame.y = 0;
    pEntity->currentFrame.w = 32;
    pEntity->currentFrame.h = 32;
    return pEntity;
}

//Get X of entity
float getX(Entity *pEntity)
{
    return pEntity->x;
}
//Get Y of entity
float getY(Entity *pEntity)
{
    return pEntity->y;
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