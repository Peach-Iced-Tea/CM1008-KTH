#include "renderWindow.h"

struct renderWindow {
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    bool fullscreen;
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

    return pRenderWindow;
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

void windowHandleInput(RenderWindow *pRenderWindow, Input const *pInputs) {
    if (checkKeyCombo(pInputs, KEY_ALT, KEY_RETURN)) { toggleFullscreen(pRenderWindow); }
    if (checkKeyCombo(pInputs, KEY_ALT, KEY_TAB) && pRenderWindow->fullscreen) { toggleFullscreen(pRenderWindow); }

    return;
}

SDL_Texture *loadTexture(RenderWindow *pRenderWindow, char const *pFilePath) {
    SDL_Texture* texture = NULL;
    texture = IMG_LoadTexture(pRenderWindow->pRenderer, pFilePath);
    if (texture == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }
    
    return texture;
}

void clearWindow(RenderWindow *pRenderWindow) {
    SDL_RenderClear(pRenderWindow->pRenderer);
    pRenderWindow->nrOfRenderedEntites = 0;
}

void adjustToCamera(Camera const *pCamera, SDL_FRect *dst, Vec2 *vector) {
    float offsetWidth = cameraGetWidth(pCamera)*0.5f;
    float offsetHeight = cameraGetHeight(pCamera)*0.5f;
    Vec2 cameraPosition = cameraGetPosition(pCamera);

    if (dst != NULL) {
        dst->x += offsetWidth - cameraPosition.x;
        dst->y += offsetHeight - cameraPosition.y;
    }

    if (vector != NULL) {
        vector->x += offsetWidth - cameraPosition.x;
        vector->y += offsetHeight - cameraPosition.y;
    }

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
    pRenderWindow->nrOfRenderedEntites++;
}

void renderPlayer(RenderWindow *pRenderWindow, Player const *pPlayer, Camera const *pCamera) {
    SDL_FRect dst = getCurrentFrame(playerGetBody(pPlayer));
    adjustToCamera(pCamera, &dst, NULL);

    SDL_Rect src = playerGetSheetPosition(pPlayer);

    SDL_RenderCopyF(pRenderWindow->pRenderer, getTexture(playerGetBody(pPlayer)), &src, &dst);
} 

void displayWindow(RenderWindow *pRenderWindow) {
    SDL_RenderPresent(pRenderWindow->pRenderer);
}

void drawLine(RenderWindow *pRenderWindow, Vec2 pos1, Vec2 pos2, Camera const *pCamera) {
    adjustToCamera(pCamera, NULL, &pos1);
    adjustToCamera(pCamera, NULL, &pos2);
    
    SDL_SetRenderDrawColor(pRenderWindow->pRenderer, 255, 255, 255, 255);
    SDL_RenderDrawLineF(pRenderWindow->pRenderer, pos1.x, pos1.y, pos2.x, pos2.y);
    SDL_SetRenderDrawColor(pRenderWindow->pRenderer, 0, 0, 0, 255);
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