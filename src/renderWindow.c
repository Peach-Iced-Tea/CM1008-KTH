#include "renderWindow.h"

struct renderWindow {
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    bool fullscreen;
    int nrOfRenderedEntites;    // Used for debugging purposes.
};

RenderWindow *createRenderWindow(const char* pTitle, int w, int h) {
    RenderWindow *rw = malloc(sizeof(RenderWindow));
    rw->pWindow = SDL_CreateWindow(pTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if(!rw->pWindow) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }

    rw->pRenderer = SDL_CreateRenderer(rw->pWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!rw->pRenderer) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }

    rw->fullscreen = false;

    return rw;
}

void toggleFullscreen(RenderWindow *pRenderWindow) {
    if (pRenderWindow->fullscreen) {
        SDL_SetWindowFullscreen(pRenderWindow->pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
        pRenderWindow->fullscreen = false;
    }
    else {
        SDL_SetWindowFullscreen(pRenderWindow->pWindow, SDL_WINDOW_FULLSCREEN);
        pRenderWindow->fullscreen = true;
    }
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
    pRenderWindow->nrOfRenderedEntites = 0;
}

void renderEntity(RenderWindow *pRenderWindow, Entity *pEntity, Camera *pCamera) {
    if (!entityIsVisible(pCamera, pEntity)) { return; }

    SDL_FRect entity = getCurrentFrame(pEntity);
    float offsetX = cameraGetWidth(pCamera)*0.5f;
    float offsetY = cameraGetHeight(pCamera)*0.5f;
    Vec2 cameraPosition = cameraGetPosition(pCamera);

    SDL_FRect dst;
    dst.w = entity.w;
    dst.h = entity.h;
    dst.x = entity.x + offsetX-cameraPosition.x - dst.w*0.5f;
    dst.y = entity.y + offsetY-cameraPosition.y - dst.h*0.5f;

    SDL_FRect src;
    src.w = dst.w;
    src.h = dst.h;
    src.x = entity.x;
    src.y = entity.y;

    SDL_RenderCopyF(pRenderWindow->pRenderer, getTexture(pEntity), NULL, &dst);
    pRenderWindow->nrOfRenderedEntites++;
}

void displayWindow(RenderWindow *pRenderWindow) {
    SDL_RenderPresent(pRenderWindow->pRenderer);
}

SDL_Renderer *getRenderer(RenderWindow *pRenderWindow) {
    return pRenderWindow->pRenderer;
}

void destroyRenderWindow(RenderWindow *pRenderWindow) {
    if (pRenderWindow == NULL) { return; }
    
    SDL_DestroyWindow(pRenderWindow->pWindow);
    SDL_DestroyRenderer(pRenderWindow->pRenderer);
}