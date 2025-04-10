#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "renderWindow.h"
#include "entity.h"

struct renderWindow {
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
};

RenderWindow *createRenderWindow(const char* p_title, int p_w, int p_h) {
    RenderWindow *rw = malloc(sizeof(RenderWindow));
    rw->pWindow = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, p_w, p_h, 0);
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

SDL_Texture *loadTexture(const char* p_filePath, struct renderWindow *pRenderWindow) {
    SDL_Texture* texture = NULL;
    texture = IMG_LoadTexture(pRenderWindow->pRenderer, p_filePath);
    if(texture == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }
    return texture;
}

void cleanUp(RenderWindow *pRenderWindow) {
    SDL_DestroyWindow(pRenderWindow->pWindow);
    SDL_DestroyRenderer(pRenderWindow->pRenderer);
}

void clearWindow(RenderWindow *pRenderWindow) {
    SDL_RenderClear(pRenderWindow->pRenderer);
}

void renderEntity(RenderWindow *pRenderWindow, Entity *p_entity) {
    SDL_Rect src;
    src.x = getCurrentFrame(p_entity).x;
    src.y = getCurrentFrame(p_entity).y;
    src.w = getCurrentFrame(p_entity).w;
    src.h = getCurrentFrame(p_entity).h;

    SDL_Rect dst;
    dst.x = round(getPosition(p_entity).x) * GLOBAL_SCALER;
    dst.y = round(getPosition(p_entity).y) * GLOBAL_SCALER;
    dst.w = getCurrentFrame(p_entity).w * GLOBAL_SCALER;
    dst.h = getCurrentFrame(p_entity).h * GLOBAL_SCALER;

    //printf("entity.x: %f, entity.y: %f\n", getPos(p_entity).x, getPos(p_entity).y);
    //printf("dst.x: %d, dst.y %d\n", dst.x, dst.y);

    SDL_RenderCopy(pRenderWindow->pRenderer, getTexture(p_entity), NULL, &dst);
}

void displayWindow(RenderWindow *pRenderWindow) {
    SDL_RenderPresent(pRenderWindow->pRenderer);
}


