#include "renderWindow.h"

struct renderWindow {
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    bool fullscreen;
    bool renderHitboxes;
    int nrOfRenderedEntites;    // Used for debugging purposes.
};

RenderWindow *createRenderWindow(const char* pTitle, int w, int h) {
    RenderWindow *pRenderWindow = malloc(sizeof(RenderWindow));
    pRenderWindow->pWindow = SDL_CreateWindow(pTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!pRenderWindow->pWindow) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }

    pRenderWindow->pRenderer = SDL_CreateRenderer(pRenderWindow->pWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if (!pRenderWindow->pRenderer) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }

    pRenderWindow->fullscreen = false;
    pRenderWindow->renderHitboxes = false;
    SDL_SetWindowIcon(pRenderWindow->pWindow, IMG_Load("resources/gameIcon.png"));

    return pRenderWindow;
}

void toggleFullscreen(RenderWindow *pRenderWindow) {
    if (pRenderWindow->fullscreen) {
        SDL_SetWindowFullscreen(pRenderWindow->pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
        printf("Fullscreen: Disabled!\n");
        pRenderWindow->fullscreen = false;
    }
    else {
        SDL_SetWindowFullscreen(pRenderWindow->pWindow, SDL_WINDOW_FULLSCREEN);
        printf("Fullscreen: Enabled!\n");
        pRenderWindow->fullscreen = true;
    }
    
    return;
}

void windowHandleInput(RenderWindow *pRenderWindow, Input const *pInputs) {
    Uint32 flags = SDL_GetWindowFlags(pRenderWindow->pWindow);
    if ((flags&SDL_WINDOW_INPUT_FOCUS)==0) {
        if (pRenderWindow->fullscreen) { toggleFullscreen(pRenderWindow); }
        SDL_MinimizeWindow(pRenderWindow->pWindow);
    }

    if (checkKeyCombo(pInputs, KEY_ALT, KEY_RETURN)) { toggleFullscreen(pRenderWindow); }
    if (checkKeyCombo(pInputs, KEY_ALT, KEY_T)) { pRenderWindow->renderHitboxes = !pRenderWindow->renderHitboxes; }

    return;
}

SDL_Texture *loadTexture(RenderWindow *pRenderWindow, char const *pFilePath) {
    SDL_Texture *pTexture = NULL;
    pTexture = IMG_LoadTexture(pRenderWindow->pRenderer, pFilePath);
    if (pTexture == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }
    
    return pTexture;
}

void clearWindow(RenderWindow *pRenderWindow) {
    SDL_RenderClear(pRenderWindow->pRenderer);
    pRenderWindow->nrOfRenderedEntites = 0;
    return;
}

void renderHitbox(RenderWindow *pRenderWindow, Hitbox const *pHitbox, Camera const *pCamera) {
    Vec2 halfSize = getHitboxHalfSize(pHitbox);
    Vec2 topLeftCorner;
    vectorSub(&topLeftCorner, getHitboxPosition(pHitbox), halfSize);
    SDL_FRect dst;
    dst.x = topLeftCorner.x;
    dst.y = topLeftCorner.y;
    dst.w = halfSize.x*2.0f;
    dst.h = halfSize.y*2.0f;
    adjustToCamera(pCamera, &dst, NULL);

    SDL_SetRenderDrawColor(pRenderWindow->pRenderer, 255, 255, 0, 255);
    SDL_RenderDrawRectF(pRenderWindow->pRenderer, &dst);
    SDL_SetRenderDrawColor(pRenderWindow->pRenderer, 0, 0, 0, 255);
    return;
}

void renderEntity(RenderWindow *pRenderWindow, Entity const *pEntity, Camera const *pCamera) {
    SDL_FRect dst = getCurrentFrame(pEntity);
    adjustToCamera(pCamera, &dst, NULL);

    SDL_Rect src;
    src.w = 32;
    src.h = 32;
    src.x = 0;
    src.y = 0;

    SDL_RenderCopyF(pRenderWindow->pRenderer, getTexture(pEntity), &src, &dst);
    if (pRenderWindow->renderHitboxes) { renderHitbox(pRenderWindow, getHitbox(pEntity), pCamera); }
    pRenderWindow->nrOfRenderedEntites++;
    return;
}

void renderPlayer(RenderWindow *pRenderWindow, Player const *pPlayer, Camera const *pCamera) {
    SDL_FRect dst = getCurrentFrame(playerGetBody(pPlayer));
    adjustToCamera(pCamera, &dst, NULL);

    SDL_Rect src = playerGetSheetPosition(pPlayer);

    SDL_RenderCopyF(pRenderWindow->pRenderer, getTexture(playerGetBody(pPlayer)), &src, &dst);
    if (pRenderWindow->renderHitboxes) { renderHitbox(pRenderWindow, getHitbox(playerGetBody(pPlayer)), pCamera); }
    return;
} 

void displayWindow(RenderWindow *pRenderWindow) {
    SDL_RenderPresent(pRenderWindow->pRenderer);
    return;
}

void drawLine(RenderWindow *pRenderWindow, Vec2 pos1, Vec2 pos2, Camera const *pCamera) {
    adjustToCamera(pCamera, NULL, &pos1);
    adjustToCamera(pCamera, NULL, &pos2);
    
    SDL_SetRenderDrawColor(pRenderWindow->pRenderer, 255, 255, 255, 255);
    SDL_RenderDrawLineF(pRenderWindow->pRenderer, pos1.x, pos1.y, pos2.x, pos2.y);
    SDL_SetRenderDrawColor(pRenderWindow->pRenderer, 0, 0, 0, 255);
    return;
}

SDL_Renderer *getRenderer(RenderWindow const *pRenderWindow) {
    return pRenderWindow->pRenderer;
}

void destroyRenderWindow(RenderWindow *pRenderWindow) {
    if (pRenderWindow == NULL) { return; }
    
    SDL_DestroyWindow(pRenderWindow->pWindow);
    SDL_DestroyRenderer(pRenderWindow->pRenderer);
    return;
}