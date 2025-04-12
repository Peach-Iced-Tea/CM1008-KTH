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

    SDL_FRect src;
    src.w = entity.w;
    src.h = entity.h;
    src.x = entity.x;
    src.y = entity.y;
    
    SDL_RenderCopyF(pRenderWindow->pRenderer, getTexture(pEntity), NULL, &dst);
}

void displayWindow(RenderWindow *pRenderWindow) {
    SDL_RenderPresent(pRenderWindow->pRenderer);
}

void destroyRenderWindow(RenderWindow *pRenderWindow) {
    if (pRenderWindow == NULL) { return; }
    
    SDL_DestroyWindow(pRenderWindow->pWindow);
    SDL_DestroyRenderer(pRenderWindow->pRenderer);
}