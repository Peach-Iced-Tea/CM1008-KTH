#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Entity.h"



struct entity {
    Vec2 *pos;
    Hitbox box;
    SDL_Rect currentFrame;
    SDL_Texture* tex;
};

Entity *createEntiy(Vec2 *p_pos, SDL_Texture *p_tex, int hitboxType) {
    Entity *pEntity = malloc(sizeof(Entity));

    pEntity->pos = p_pos;

    pEntity->tex = p_tex;

    pEntity->currentFrame.x = 0;//p_pos.x;
    pEntity->currentFrame.y = 0;//p_pos.y;
    pEntity->currentFrame.w = 32;
    pEntity->currentFrame.h = 32;

    switch(hitboxType) {
        case HITBOX_PLAYER:
            createPlayerHitbox(&pEntity->box, p_pos, (float)pEntity->currentFrame.w, (float)pEntity->currentFrame.h);
            break;
        case HITBOX_32BLOCK:
            create32BlockHitbox(&pEntity->box, p_pos, (float)pEntity->currentFrame.w, (float)pEntity->currentFrame.h);
            break;
    }

    return pEntity;
}

void createPlayerHitbox(Hitbox *pBox, Vec2 *pPos, float w, float h) {
    pBox->w = w/2;
    pBox->h = h/2;
    pBox->x = vectorGetX(pPos) + (pBox->w - (pBox->w/2));
    pBox->y = vectorGetY(pPos) + pBox->h;
}

void create32BlockHitbox(Hitbox *pBox, Vec2 *pPos, float w, float h) {
    pBox->w = w;
    pBox->h = h;
    pBox->x = vectorGetX(pPos);
    pBox->y = vectorGetY(pPos);
}

Hitbox getHPos(Entity *pEntity) {
    return pEntity->box;
}

/*Vec2 getPos(Entity *pEntity) {
    //Vec2 *vector = malloc(sizeof(Vec2*));
    return pEntity->pos;
}*/


//returns texture of given Entity
SDL_Texture *getTex(Entity *pEntity) {
    return pEntity->tex;
}
//returns currentFrame of given Entity
SDL_Rect getCurrentFrame(Entity *pEntity) {
    return pEntity->currentFrame;
}


void movePlayer(Entity *pEntity, Vec2 *vector) {
    //printf("x: %f || y: %f\n", x, y);
    //pEntity->pos.vx = x;
    //pEntity->pos.vy = y;
    vectorAdd(pEntity->pos, pEntity->pos, vector);
    //pEntity->pos.x +=x;
    //pEntity->pos.y +=y;

    pEntity->box.x += vectorGetX(vector);
    pEntity->box.y += vectorGetY(vector);

    //pEntity->currentFrame.x = round(pEntity->pos.x);
    //pEntity->currentFrame.y = round(pEntity->pos.y);
    
    /* Anledningen till att det blir så fucked är för att jag
    flyttar entities med delta tid floats som är typ 0,00513 liksom
    och sen gör de till ints.... */
}


void updateEntity(Entity *pEntity) {
    //Have to update struct Rect for the entitie
    pEntity->currentFrame.x = 0.0; 
    pEntity->currentFrame.y = 0.0;
    pEntity->box.x = 0.0;
    pEntity->box.y = 0.0;
    //pEntity->pos.x = 0.0;
    //pEntity->pos.y = 0.0;

    //printf("x: %f || y: %f\n", pEntity->pos.vx, pEntity->pos.vy);
    //pEntity->pos.y +=pEntity->pos.vy;
    //pEntity->pos.x +=pEntity->pos.vx;
    //pEntity->pos.vx = 0;
    //pEntity->pos.vy = 0;
}

float entityGetX(Entity *pEntity) {
    return vectorGetX(pEntity->pos);
}

float entityGetY(Entity *pEntity) {
    return vectorGetY(pEntity->pos);
}

int checkIntersection(Entity *pPlayer, Entity *pEntity, float directionX, float directionY) {
    float x1,x2,y1,y2;
    x1 = pPlayer->box.x;
    x2 = pEntity->box.x;
    y1 = pPlayer->box.y;
    y2 = pEntity->box.y;
    if (x1+pPlayer->box.w<x2 || x1>x2+pEntity->box.w) {
        return 0;
    }

    if (y1+pPlayer->box.h<y2 || y1>y2+pEntity->box.h) {
        return 0;
    }

    Vec2 *tmpVec = createVector();
    // Vertical movement:
    if (directionY>0.0f && y1<y2) {     // Player has moved downwards, also check that the player is above the object.
        float tmp = y1+pPlayer->box.h-y2;
        if (tmp>0) {
            pPlayer->box.y-=(tmp+0.01);
            initVec(tmpVec, 0.0f, tmp+0.01);
            vectorSub(pPlayer->pos, pPlayer->pos, tmpVec);
        }
        return BLOCK_DOWN;
    }
    else if (directionY<0.0f && y1>y2) {    // Player has moved upwards, also check that the player is below the object.
        float tmp = y2+pEntity->box.h-y1;
        if (tmp>0) {
            pPlayer->box.y+=(tmp+0.01);
            initVec(tmpVec, 0.0f, tmp+0.01);
            vectorAdd(pPlayer->pos, pPlayer->pos, tmpVec);
        }
        return BLOCK_UP;
    }

    // Horizontal movement:
    if (directionX>0.0f && x1<x2) {     // Player has moved to the right, also check that the player is to the left of the object.
        float tmp = x1+pPlayer->box.w-x2;
        if (tmp>0) {
            pPlayer->box.x-=(tmp+0.01);
            initVec(tmpVec, tmp+0.01, 0.0f);
            vectorSub(pPlayer->pos, pPlayer->pos, tmpVec);
        }
        return BLOCK_RIGHT;
    }
    else if (directionX<0.0f && x1>x2) {    // Player has moved to the left, also check that the player is to the right of the object.
        float tmp = x2+pEntity->box.w-x1;
        if (tmp>0) {
            pPlayer->box.x+=(tmp+0.01);
            initVec(tmpVec, tmp+0.01, 0.0f);
            vectorAdd(pPlayer->pos, pPlayer->pos, tmpVec);
        }
        return BLOCK_LEFT;
    }

    return 0;
}
