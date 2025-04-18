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
    float offsetWidth = cameraGetWidth(pCamera)*0.5f;
    float offsetHeight = cameraGetHeight(pCamera)*0.5f;
    Vec2 cameraPosition = cameraGetPosition(pCamera);

    SDL_FRect dst = getCurrentFrame(pEntity);
    dst.x += offsetWidth - cameraPosition.x;
    dst.y += offsetHeight - cameraPosition.y;

    SDL_Rect src;
    src.w = 32;
    src.h = 32;
    src.x = 0;
    src.y = 0;
/* 
    SDL_Rect src;
    src.w = 32.0f;
    src.h = 32.0f;
    src.x = 0;
    src.y = 0;
  */   
    SDL_RenderCopyF(pRenderWindow->pRenderer, getTexture(pEntity), &src, &dst);
    pRenderWindow->nrOfRenderedEntites++;
}

void renderPlayer(RenderWindow *pRenderWindow, Player *pPlayer, Camera *pCamera) {
    float offsetWidth = cameraGetWidth(pCamera)*0.5f;
    float offsetHeight = cameraGetHeight(pCamera)*0.5f;
    Vec2 cameraPosition = cameraGetPosition(pCamera);


    //SDL_FRect entity = getCurrentFrame(playerGetBody(pPlayer));

    SDL_FRect dst = getCurrentFrame(playerGetBody(pPlayer));
    dst.x += offsetWidth - cameraPosition.x;
    dst.y += offsetHeight - cameraPosition.y;

/* 
    SDL_FRect dst;
    dst.w = entity.w * GLOBAL_SCALER;
    dst.h = entity.h * GLOBAL_SCALER;
    dst.x = entity.x * GLOBAL_SCALER;
    dst.y = entity.y * GLOBAL_SCALER;
 */
    SDL_Rect src = playerGetSheetPosition(pPlayer);

    SDL_RenderCopyF(pRenderWindow->pRenderer, getTexture(playerGetBody(pPlayer)), &src, &dst);
} 

void displayWindow(RenderWindow *pRenderWindow) {
    SDL_RenderPresent(pRenderWindow->pRenderer);
}

void drawLine(RenderWindow *pRenderWindow, Vec2 pos1, Vec2 pos2, Camera *pCamera) {
    float offsetWidth = cameraGetWidth(pCamera)*0.5f;
    float offsetHeight = cameraGetHeight(pCamera)*0.5f;
    Vec2 cameraPosition = cameraGetPosition(pCamera);


    SDL_SetRenderDrawColor(pRenderWindow->pRenderer, 255, 255, 255, 255);
    SDL_RenderDrawLineF(pRenderWindow->pRenderer, pos1.x + offsetWidth - cameraPosition.x, pos1.y + offsetHeight - cameraPosition.y, pos2.x + offsetWidth - cameraPosition.x, pos2.y + offsetHeight - cameraPosition.y);
    SDL_SetRenderDrawColor(pRenderWindow->pRenderer, 0, 0, 0, 255);
}

SDL_Renderer *getRenderer(RenderWindow *pRenderWindow) {
    return pRenderWindow->pRenderer;
}

void destroyRenderWindow(RenderWindow *pRenderWindow) {
    if (pRenderWindow == NULL) { return; }
    
    SDL_DestroyWindow(pRenderWindow->pWindow);
    SDL_DestroyRenderer(pRenderWindow->pRenderer);
}