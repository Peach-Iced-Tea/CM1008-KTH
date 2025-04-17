#include "renderWindow.h"

struct renderWindow {
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
};

RenderWindow *createRenderWindow(const char* pTitle, int w, int h) {
    RenderWindow *rw = malloc(sizeof(RenderWindow));
    rw->pWindow = SDL_CreateWindow(pTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, 0);
    if(!rw->pWindow) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }
   
    rw->pRenderer = SDL_CreateRenderer(rw->pWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!rw->pRenderer) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }

   return rw;
}

SDL_Texture *loadTexture(RenderWindow *pRenderWindow, const char* pFilePath) {
    SDL_Texture* texture = NULL;
    texture = IMG_LoadTexture(pRenderWindow->pRenderer, pFilePath);
    if(texture == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }
    
    return texture;
}

void clearWindow(RenderWindow *pRenderWindow) {
    SDL_RenderClear(pRenderWindow->pRenderer);
}

void renderEntity(RenderWindow *pRenderWindow, Entity *pEntity) {
    SDL_FRect entity = getCurrentFrame(pEntity);

    SDL_FRect dst;
    dst.w = entity.w * GLOBAL_SCALER;
    dst.h = entity.h * GLOBAL_SCALER;
    dst.x = entity.x * GLOBAL_SCALER;
    dst.y = entity.y * GLOBAL_SCALER;

    SDL_Rect src;
    src.w = 32.0f;
    src.h = 32.0f;
    src.x = 0;
    src.y = 0;
    
    SDL_RenderCopyF(pRenderWindow->pRenderer, getTexture(pEntity), &src, &dst);
}

void displayWindow(RenderWindow *pRenderWindow) {
    SDL_RenderPresent(pRenderWindow->pRenderer);
}

void destroyRenderWindow(RenderWindow *pRenderWindow) {
    if (pRenderWindow == NULL) { return; }
    
    SDL_DestroyWindow(pRenderWindow->pWindow);
    SDL_DestroyRenderer(pRenderWindow->pRenderer);
}


//Fixa får inte ligga här
void drawLine(RenderWindow *pRenderWindow, Vec2 pos1, Vec2 pos2) {
    SDL_SetRenderDrawColor(pRenderWindow->pRenderer, 255, 255, 255, 255);
    SDL_RenderDrawLineF(pRenderWindow->pRenderer, pos1.x *GLOBAL_SCALER, pos1.y * GLOBAL_SCALER, pos2.x * GLOBAL_SCALER, pos2.y * GLOBAL_SCALER);
    SDL_SetRenderDrawColor(pRenderWindow->pRenderer, 0, 0, 0, 255);
}

void renderPlayer(RenderWindow *pRenderWindow, Entity *pEntity) {
    SDL_FRect entity = getCurrentFrame(pEntity);

    SDL_FRect dst;
    dst.w = entity.w * GLOBAL_SCALER;
    dst.h = entity.h * GLOBAL_SCALER;
    dst.x = entity.x * GLOBAL_SCALER;
    dst.y = entity.y * GLOBAL_SCALER;

    SDL_Rect src;
    src.w = 32.0f;
    src.h = 32.0f;
    src.x = 0;
    src.y = 0;

    SDL_RenderCopyF(pRenderWindow->pRenderer, getTexture(pEntity), &src, &dst);
}