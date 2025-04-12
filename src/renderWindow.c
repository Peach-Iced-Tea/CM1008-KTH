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
    SDL_Rect src;
    src.x = getCurrentFrame(pEntity).x;
    src.y = getCurrentFrame(pEntity).y;
    src.w = getCurrentFrame(pEntity).w;
    src.h = getCurrentFrame(pEntity).h;

    SDL_Rect dst;
    dst.x = round(getPosition(pEntity).x) * GLOBAL_SCALER;
    dst.y = round(getPosition(pEntity).y) * GLOBAL_SCALER;
    dst.w = getCurrentFrame(pEntity).w * GLOBAL_SCALER;
    dst.h = getCurrentFrame(pEntity).h * GLOBAL_SCALER;

    SDL_RenderCopy(pRenderWindow->pRenderer, getTexture(pEntity), NULL, &dst);
}

void displayWindow(RenderWindow *pRenderWindow) {
    SDL_RenderPresent(pRenderWindow->pRenderer);
}

void destroyRenderWindow(RenderWindow *pRenderWindow) {
    if (pRenderWindow == NULL) { return; }
    
    SDL_DestroyWindow(pRenderWindow->pWindow);
    SDL_DestroyRenderer(pRenderWindow->pRenderer);
}