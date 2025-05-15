#include "platforms.h"

struct movablePlatform {
    Entity *pPlatform[MAX_PLATFORM_SIZE];
    Vec2 velocity;
    SDL_Rect sheetPosition;
    int platformSize;
    int platformDirection;
};

MovablePlatform *createPlatform(){
    MovablePlatform *pPlatform = malloc(sizeof(MovablePlatform));
    return pPlatform;
}

void initMovablePlatforms(SDL_Renderer *pWindow, MovablePlatform *pPlatformOne){
    Vec2 platformInitPos;
    SDL_Texture *pPlatformTexture = NULL;
    if(pWindow != NULL){
        pPlatformTexture = IMG_LoadTexture(pWindow, "lib/resources/player1.png");
    }
    pPlatformOne->platformSize = MAX_MOVE_PLATFORMS;
    platformInitPos.x = 192.0f;
    platformInitPos.y = 992.0f;
    for(int i = 0;i < pPlatformOne->platformSize; i++){
        if(i == 5){
            platformInitPos.y += 32.0f;
            platformInitPos.x = 224.0f;
        }
        pPlatformOne->pPlatform[i] = createEntity(platformInitPos, pPlatformTexture, 10, HITBOX_FULL_BLOCK);
        pPlatformOne->velocity.x = PLATFORM_SPEED;
        pPlatformOne->velocity.y = 0.0f;
        pPlatformOne->sheetPosition.x = 0.0f;
        pPlatformOne->sheetPosition.y = 0.0f;
        platformInitPos.x += 32.0f;
    }
}

void movePlatforms(MovablePlatform *pPlatformOne){
    Vec2 pos1 = entityGetPosition(pPlatformOne->pPlatform[0]);
    Vec2 pos2 = entityGetPosition(pPlatformOne->pPlatform[4]);

    if(pos2.x >= 512){
        pPlatformOne->velocity.x = -PLATFORM_SPEED;
    }
    else if(pos1.x <= 192){
        pPlatformOne->velocity.x = PLATFORM_SPEED;
    }

    for(int i = 0; i < pPlatformOne->platformSize; i++){
        entityMove(pPlatformOne->pPlatform[i], pPlatformOne->velocity);
    }
}

Entity *platformGetEntity(MovablePlatform *pPlatform, int index){
    return pPlatform->pPlatform[index];
}

Vec2 platformGetVelocity(MovablePlatform *pPlatform){
    return pPlatform->velocity;
}

void destroyPlatform(MovablePlatform *pPlatform){
    free(pPlatform);
}