#include "renderWindow.h"

typedef enum {
    WINDOWED, BORDERLESS, FULLSCREEN, EXCLUSIVE, ALT_TABBED
} WindowState;

typedef struct textures {
    SDL_Texture *pPlayer1;
    SDL_Texture *pPlayer2;
    SDL_Texture *pTongue;
    SDL_Texture *pCrosshair;
    SDL_Texture *pObstacles;
} Textures;

struct renderWindow {
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    TTF_Font *pFont;
    WindowState state;
    WindowState lastState;
    Textures textures;
    Camera *pCamera;
    int width;
    int height;
    bool renderHitboxes;
    int nrOfRenderedEntites;    // Used for debugging purposes.
};

int loadTextures(Textures *pTextures, SDL_Renderer *pRenderer) {
    pTextures->pPlayer1 = IMG_LoadTexture(pRenderer, "lib/resources/spriteSheetPlayer.png");
    if (pTextures->pPlayer1 == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
    pTextures->pPlayer2 = IMG_LoadTexture(pRenderer, "lib/resources/spriteSheetPlayer.png");
    if (pTextures->pPlayer2 == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
    pTextures->pTongue = IMG_LoadTexture(pRenderer, "lib/resources/tongue.png");
    if (pTextures->pTongue == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
    pTextures->pCrosshair = IMG_LoadTexture(pRenderer, "lib/resources/crosshair.png");
    if (pTextures->pCrosshair == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
    pTextures->pObstacles = IMG_LoadTexture(pRenderer, "lib/resources/obstacles.png");
    if (pTextures->pObstacles == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    return 0;
}

RenderWindow *createRenderWindow(const char* pTitle, int w, int h) {
    RenderWindow *pWindow = malloc(sizeof(RenderWindow));
    pWindow->pWindow = SDL_CreateWindow(pTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!pWindow->pWindow) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }

    pWindow->pRenderer = SDL_CreateRenderer(pWindow->pWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if (!pWindow->pRenderer) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }

    pWindow->pFont = TTF_OpenFont("lib/resources/arial.ttf", 100);
    if (!pWindow->pFont) {
        printf("Error: %s\n", TTF_GetError());
        return NULL;
    }

    pWindow->state = FULLSCREEN;
    pWindow->lastState = pWindow->state;

    if (loadTextures(&(pWindow->textures), pWindow->pRenderer)) { return NULL; }

    pWindow->pCamera = NULL;
    pWindow->width = w;
    pWindow->height = h;
    pWindow->renderHitboxes = false;
    SDL_SetWindowIcon(pWindow->pWindow, IMG_Load("lib/resources/gameIcon.png"));

    return pWindow;
}

void toggleFullscreen(RenderWindow *pWindow) {
    switch (pWindow->state) {
        case WINDOWED:
            SDL_SetWindowFullscreen(pWindow->pWindow, 0);
            SDL_SetWindowBordered(pWindow->pWindow, SDL_TRUE);
            printf("Window State: Windowed\n");
            break;
        case BORDERLESS:
            SDL_SetWindowBordered(pWindow->pWindow, SDL_FALSE);
            printf("Window State: Borderless\n");
            break;
        case FULLSCREEN:
            SDL_SetWindowFullscreen(pWindow->pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
            printf("Window State: Fullscreen\n");
            break;
        case EXCLUSIVE:
            SDL_SetWindowFullscreen(pWindow->pWindow, SDL_WINDOW_FULLSCREEN);
            printf("Window State: Exclusive Fullscreen\n");
            break;
    }
    
    return;
}

void windowHandleInput(RenderWindow *pWindow, Input const *pInput) {
    Uint32 flags = SDL_GetWindowFlags(pWindow->pWindow);
    if ((flags&SDL_WINDOW_INPUT_FOCUS)==0) {
        switch (pWindow->state) {
            case EXCLUSIVE:
                pWindow->lastState = pWindow->state;
                pWindow->state = WINDOWED;
                toggleFullscreen(pWindow);
                pWindow->state = ALT_TABBED;
            case FULLSCREEN:
                if ((flags&SDL_WINDOW_MINIMIZED)==0) { SDL_MinimizeWindow(pWindow->pWindow); }
                break;
        }
    }
    else {
        switch (pWindow->state) {
            case ALT_TABBED:
                pWindow->state = pWindow->lastState;
                toggleFullscreen(pWindow);
                break;
        }
    }

    if (checkKeyCombo(pInput, KEY_ALT, KEY_RETURN)) {
        pWindow->state++;
        if (pWindow->state >= ALT_TABBED) { pWindow->state = WINDOWED; }
        toggleFullscreen(pWindow);
    }
    if (checkKeyCombo(pInput, KEY_ALT, KEY_T)) { pWindow->renderHitboxes = !pWindow->renderHitboxes; }

    return;
}

SDL_Texture *windowGetTexture(Textures textures, RenderType renderType) {
    switch (renderType) {
        case RENDER_PLAYER1:
            return textures.pPlayer1;
        case RENDER_PLAYER2:
            return textures.pPlayer2;
        case RENDER_TONGUE:
            return textures.pTongue;
        case RENDER_CROSSHAIR:
            return textures.pCrosshair;
        case RENDER_OBSTACLE:
            return textures.pObstacles;
    }

    return NULL;
}

void windowRenderHitbox(RenderWindow *pWindow, Hitbox const *pHitbox) {
    if (pWindow->pCamera == NULL) {
        printf("Error: RenderWindow is missing Camera\n");
        return;
    }

    if (pHitbox == NULL) { return; }

    if (!pWindow->renderHitboxes) { return; }

    Vec2 halfSize = hitboxGetHalfSize(pHitbox);
    Vec2 topLeftCorner;
    vectorSub(&topLeftCorner, hitboxGetPosition(pHitbox), halfSize);
    SDL_FRect dst;
    dst.x = topLeftCorner.x;
    dst.y = topLeftCorner.y;
    dst.w = halfSize.x*2.0f;
    dst.h = halfSize.y*2.0f;
    cameraAdjustToViewport(pWindow->pCamera, &dst, NULL);

    SDL_SetRenderDrawColor(pWindow->pRenderer, 255, 255, 0, 255);
    SDL_RenderDrawRectF(pWindow->pRenderer, &dst);
    SDL_SetRenderDrawColor(pWindow->pRenderer, 0, 0, 0, 255);
    return;
}

void windowRenderMenu(RenderWindow *pWindow, SDL_Texture *pTexture, SDL_Rect menuButtons[], SDL_Rect menuPosition[], int nrOfButtons) {
    for (int i = 0; i < nrOfButtons; i++) {
        SDL_Rect src = menuButtons[i];
        SDL_Rect dst = menuPosition[i];

        SDL_RenderCopy(pWindow->pRenderer, pTexture, &src, &dst);
    }
    
    return;
}

void windowRenderText(RenderWindow *pWindow, char const textToRender[], int x, int y) {
    SDL_Color color = { 255, 255, 255 };
    SDL_Surface *pSurface = TTF_RenderText_Solid(pWindow->pFont, textToRender, color);
    if (pSurface == NULL) {
        printf("Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pWindow->pRenderer, pSurface);
    if (pTexture == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return;
    }

    SDL_Rect dst;
    SDL_QueryTexture(pTexture, NULL, NULL, &dst.w, &dst.h);
    dst.x = x-dst.w*0.5f;
    dst.y = y-dst.h*0.5f;
    SDL_RenderSetLogicalSize(pWindow->pRenderer, pWindow->width, pWindow->height);
    SDL_RenderCopy(pWindow->pRenderer, pTexture, NULL, &dst);
    return;
}

void windowRenderObject(RenderWindow *pWindow, Entity const entity, RenderType renderType) {
    if (pWindow->pCamera == NULL) {
        printf("Error: RenderWindow is missing Camera\n");
        return;
    }

    SDL_FRect dst = entity.frame;
    SDL_Rect src = entity.source;
    cameraAdjustToViewport(pWindow->pCamera, &dst, NULL);

    SDL_Texture *pTexture = windowGetTexture(pWindow->textures, renderType);
    if (pTexture == NULL) { return; }

    SDL_RenderCopyExF(pWindow->pRenderer, pTexture, &src, &dst, entity.angle, NULL, entity.flip);
    if (pWindow->renderHitboxes) { windowRenderHitbox(pWindow, entity.pHitbox); }
    pWindow->nrOfRenderedEntites++;
    return;
}

void windowRenderMapLayer(RenderWindow *pWindow, ClientMap *pMap) {
    if (pWindow->pCamera == NULL) {
        printf("Error: RenderWindow is missing Camera\n");
        return;
    }

    int mapWidth = mapGetWidth(pMap);
    int tileSize = 32;

    for (size_t i = 0; i < mapGetLayerSize(pMap, 1); i++) {

        int gid = mapGetLayerData(pMap, 1, i) - 15;

        if (gid >= 0) {
            int columns = atoi((char *)mapGetColumns(mapGetTileset(pMap)));
            
            int tileX = (gid % columns) * 32;
            int tileY = (gid / columns) * 32;

            mapSetTileSheetPosition(pMap, tileX, tileY);

            int screenX = (i % mapWidth) * tileSize;
            int screenY = (i / mapWidth) * tileSize;

            SDL_Rect src = mapGetTileSheetPosition(pMap);

            SDL_FRect dst = { (float)screenX, (float)screenY, tileSize, tileSize }; 

            cameraAdjustToViewport(pWindow->pCamera, &dst, NULL);

            SDL_RenderCopyF(pWindow->pRenderer, mapGetTileTextures(mapGetTileset(pMap)), &src, &dst);
        }
    }
}

void windowClearFrame(RenderWindow *pWindow) {
    SDL_RenderClear(pWindow->pRenderer);
    pWindow->nrOfRenderedEntites = 0;
    return;
}

void windowDisplayFrame(RenderWindow *pWindow) {
    SDL_RenderPresent(pWindow->pRenderer);
    return;
}

void windowDrawLine(RenderWindow *pWindow, Vec2 pos1, Vec2 pos2) {
    if (pWindow->pCamera == NULL) {
        printf("Error: RenderWindow is missing Camera\n");
        return;
    }

    cameraAdjustToViewport(pWindow->pCamera, NULL, &pos1);
    cameraAdjustToViewport(pWindow->pCamera, NULL, &pos2);
    
    SDL_SetRenderDrawColor(pWindow->pRenderer, 255, 0, 0, 255);
    SDL_RenderDrawLineF(pWindow->pRenderer, pos1.x, pos1.y, pos2.x, pos2.y);
    SDL_SetRenderDrawColor(pWindow->pRenderer, 0, 0, 0, 255);
    return;
}

void windowSetCamera(RenderWindow *pWindow, Camera *pCamera) {
    if (pCamera == NULL) { return; }

    pWindow->pCamera = pCamera;
    return;
}

SDL_Renderer *windowGetRenderer(RenderWindow const *pWindow) {
    return pWindow->pRenderer;
}

int windowGetWidth(RenderWindow const *pWindow) {
    return pWindow->width;
}

int windowGetHeight(RenderWindow const *pWindow) {
    return pWindow->height;
}

void destroyRenderWindow(RenderWindow *pWindow) {
    if (pWindow == NULL) { return; }
    
    SDL_DestroyWindow(pWindow->pWindow);
    SDL_DestroyRenderer(pWindow->pRenderer);
    free(pWindow);
    return;
}
