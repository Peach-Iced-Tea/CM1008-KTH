#include "renderWindow.h"

typedef enum {
    WINDOWED, BORDERLESS, FULLSCREEN, EXCLUSIVE, ALT_TABBED
} WindowState;

typedef struct textures {
    SDL_Texture *pPlayer1;
    SDL_Texture *pPlayer2;
    SDL_Texture *pTongueTip;
    SDL_Texture *pTongueShaft;
    SDL_Texture *pCrosshair;
} Textures;

struct renderWindow {
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    TTF_Font *pFont;
    WindowState state;
    WindowState lastState;
    Textures textures;
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
    pTextures->pTongueTip = IMG_LoadTexture(pRenderer, "lib/resources/tongueTip.png");
    if (pTextures->pTongueTip == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
    pTextures->pTongueShaft = IMG_LoadTexture(pRenderer, "lib/resources/tongueShaft.png");
    if (pTextures->pTongueShaft == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
    pTextures->pCrosshair = IMG_LoadTexture(pRenderer, "lib/resources/crosshair.png");
    if (pTextures->pCrosshair == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    return 0;
}

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

    pRenderWindow->pFont = TTF_OpenFont("lib/resources/arial.ttf", 100);
    if (!pRenderWindow->pFont) {
        printf("Error: %s\n", TTF_GetError());
        return NULL;
    }

    pRenderWindow->state = FULLSCREEN;
    pRenderWindow->lastState = pRenderWindow->state;

    if (loadTextures(&(pRenderWindow->textures), pRenderWindow->pRenderer)) { return NULL; }

    pRenderWindow->width = w;
    pRenderWindow->height = h;
    pRenderWindow->renderHitboxes = false;
    SDL_SetWindowIcon(pRenderWindow->pWindow, IMG_Load("lib/resources/gameIcon.png"));

    return pRenderWindow;
}

void toggleFullscreen(RenderWindow *pRenderWindow) {
    switch (pRenderWindow->state) {
        case WINDOWED:
            SDL_SetWindowFullscreen(pRenderWindow->pWindow, 0);
            SDL_SetWindowBordered(pRenderWindow->pWindow, SDL_TRUE);
            printf("Window State: Windowed\n");
            break;
        case BORDERLESS:
            SDL_SetWindowBordered(pRenderWindow->pWindow, SDL_FALSE);
            printf("Window State: Borderless\n");
            break;
        case FULLSCREEN:
            SDL_SetWindowFullscreen(pRenderWindow->pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
            printf("Window State: Fullscreen\n");
            break;
        case EXCLUSIVE:
            SDL_SetWindowFullscreen(pRenderWindow->pWindow, SDL_WINDOW_FULLSCREEN);
            printf("Window State: Exclusive Fullscreen\n");
            break;
    }
    
    return;
}

void windowHandleInput(RenderWindow *pRenderWindow, Input const *pInputs) {
    Uint32 flags = SDL_GetWindowFlags(pRenderWindow->pWindow);
    if ((flags&SDL_WINDOW_INPUT_FOCUS)==0) {
        switch (pRenderWindow->state) {
            case EXCLUSIVE:
                pRenderWindow->lastState = pRenderWindow->state;
                pRenderWindow->state = WINDOWED;
                toggleFullscreen(pRenderWindow);
                pRenderWindow->state = ALT_TABBED;
            case FULLSCREEN:
                if ((flags&SDL_WINDOW_MINIMIZED)==0) { SDL_MinimizeWindow(pRenderWindow->pWindow); }
                break;
        }
    }
    else {
        switch (pRenderWindow->state) {
            case ALT_TABBED:
                pRenderWindow->state = pRenderWindow->lastState;
                toggleFullscreen(pRenderWindow);
                break;
        }
    }

    if (checkKeyCombo(pInputs, KEY_ALT, KEY_RETURN)) {
        pRenderWindow->state++;
        if (pRenderWindow->state >= ALT_TABBED) { pRenderWindow->state = WINDOWED; }
        toggleFullscreen(pRenderWindow);
    }
    if (checkKeyCombo(pInputs, KEY_ALT, KEY_T)) { pRenderWindow->renderHitboxes = !pRenderWindow->renderHitboxes; }

    return;
}

SDL_Texture *windowLoadTexture(RenderWindow *pRenderWindow, char const *pFilePath) {
    SDL_Texture *pTexture = NULL;
    pTexture = IMG_LoadTexture(pRenderWindow->pRenderer, pFilePath);
    if (pTexture == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }
    
    return pTexture;
}

void windowRenderHitbox(RenderWindow *pRenderWindow, Hitbox const *pHitbox, Camera const *pCamera) {
    if (!pRenderWindow->renderHitboxes) { return; }

    Vec2 halfSize = getHitboxHalfSize(pHitbox);
    Vec2 topLeftCorner;
    vectorSub(&topLeftCorner, getHitboxPosition(pHitbox), halfSize);
    SDL_FRect dst;
    dst.x = topLeftCorner.x;
    dst.y = topLeftCorner.y;
    dst.w = halfSize.x*2.0f;
    dst.h = halfSize.y*2.0f;
    cameraAdjustToViewport(pCamera, &dst, NULL);

    SDL_SetRenderDrawColor(pRenderWindow->pRenderer, 255, 255, 0, 255);
    SDL_RenderDrawRectF(pRenderWindow->pRenderer, &dst);
    SDL_SetRenderDrawColor(pRenderWindow->pRenderer, 0, 0, 0, 255);
    return;
}

void windowRenderMenu(RenderWindow *pRenderWindow, SDL_Texture *pTexture, SDL_Rect menuButtons[], SDL_Rect menuPosition[], int nrOfButtons) {
    for (int i = 0; i < nrOfButtons; i++) {
        SDL_Rect src = menuButtons[i];
        SDL_Rect dst = menuPosition[i];

        SDL_RenderCopy(pRenderWindow->pRenderer, pTexture, &src, &dst);
    }
    
    return;
}

void windowRenderText(RenderWindow *pRenderWindow, char const textToRender[], int x, int y) {
    SDL_Color color = { 255, 255, 255 };
    SDL_Surface *pSurface = TTF_RenderText_Solid(pRenderWindow->pFont, textToRender, color);
    if (pSurface == NULL) {
        printf("Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderWindow->pRenderer, pSurface);
    if (pTexture == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return;
    }

    SDL_Rect dst;
    SDL_QueryTexture(pTexture, NULL, NULL, &dst.w, &dst.h);
    dst.x = x-dst.w*0.5f;
    dst.y = y-dst.h*0.5f;
    SDL_RenderCopy(pRenderWindow->pRenderer, pTexture, NULL, &dst);
    return;
}

void windowRenderEntity(RenderWindow *pRenderWindow, Entity const *pEntity, Camera const *pCamera) {
    SDL_FRect dst = entityGetCurrentFrame(pEntity);
    cameraAdjustToViewport(pCamera, &dst, NULL);

    SDL_Rect src;
    src.w = 32;
    src.h = 32;
    src.x = 0;
    src.y = 0;

    SDL_RenderCopyF(pRenderWindow->pRenderer, entityGetTexture(pEntity), &src, &dst);
    if (pRenderWindow->renderHitboxes) { windowRenderHitbox(pRenderWindow, entityGetHitbox(pEntity), pCamera); }
    pRenderWindow->nrOfRenderedEntites++;
    return;
}

void windowRenderTongue(RenderWindow *pRenderWindow, Tongue const *pTongue, Camera const *pCamera) {
    TongueInfo info = tongueGetInfo(pTongue);
    SDL_FRect dst = info.tongueShaft;
    cameraAdjustToViewport(pCamera, &dst, NULL);
    float angle = info.angle * (180.0f/M_PI);
    dst.x -= dst.w*0.5f;
    dst.y -= dst.h*0.5f;

    SDL_RenderCopyExF(pRenderWindow->pRenderer, pRenderWindow->textures.pTongueShaft, NULL, &dst, angle, NULL, 0);

    dst = entityGetCurrentFrame(tongueGetTip(pTongue));
    cameraAdjustToViewport(pCamera, &dst, NULL);
    SDL_Rect src;
    src.w = 32;
    src.h = 32;
    src.x = 0;
    src.y = 0;

    SDL_RenderCopyF(pRenderWindow->pRenderer, pRenderWindow->textures.pTongueTip, &src, &dst);
    if (pRenderWindow->renderHitboxes) { windowRenderHitbox(pRenderWindow, entityGetHitbox(tongueGetTip(pTongue)), pCamera); }
    return;
}

void windowRenderPlayer(RenderWindow *pRenderWindow, Player const *pPlayer, Camera const *pCamera) {
    SDL_FRect dst = entityGetCurrentFrame(playerGetBody(pPlayer));
    cameraAdjustToViewport(pCamera, &dst, NULL);

    switch (playerGetInfo(pPlayer).state) {
        case SHOOTING:
        case RELEASE:
        case ROTATING:
            windowRenderTongue(pRenderWindow, playerGetTongue(pPlayer), pCamera);
            break;
    }

    SDL_Rect src = playerGetInfo(pPlayer).sheetPosition;
    SDL_RenderCopyF(pRenderWindow->pRenderer, pRenderWindow->textures.pPlayer1, &src, &dst);
    if (pRenderWindow->renderHitboxes) { windowRenderHitbox(pRenderWindow, playerGetBodyHitbox(pPlayer), pCamera); }
    return;
}

void windowRenderMapLayer(RenderWindow *pRenderWindow, ClientMap *pMap, Camera const *pCamera) {
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

            cameraAdjustToViewport(pCamera, &dst, NULL);

            SDL_RenderCopyF(pRenderWindow->pRenderer, mapGetTileTextures(mapGetTileset(pMap)), &src, &dst);
        }
    }
}

void windowRenderCrosshair(RenderWindow *pRenderWindow, Crosshair const *pCrosshair, Camera *pCamera) {
    Entity *pEntity = crosshairGetBody(pCrosshair);
    SDL_Rect src = crosshairGetSheetPosition(pCrosshair);
    SDL_FRect dst = entityGetCurrentFrame(pEntity);
    cameraAdjustToViewport(pCamera, &dst, NULL);

    SDL_RenderCopyF(pRenderWindow->pRenderer, pRenderWindow->textures.pCrosshair, &src, &dst);
    return;
}

void windowClearFrame(RenderWindow *pRenderWindow) {
    SDL_RenderClear(pRenderWindow->pRenderer);
    pRenderWindow->nrOfRenderedEntites = 0;
    return;
}

void windowDisplayFrame(RenderWindow *pRenderWindow) {
    SDL_RenderPresent(pRenderWindow->pRenderer);
    return;
}

void windowDrawLine(RenderWindow *pRenderWindow, Vec2 pos1, Vec2 pos2, Camera const *pCamera) {
    cameraAdjustToViewport(pCamera, NULL, &pos1);
    cameraAdjustToViewport(pCamera, NULL, &pos2);
    
    SDL_SetRenderDrawColor(pRenderWindow->pRenderer, 255, 0, 0, 255);
    SDL_RenderDrawLineF(pRenderWindow->pRenderer, pos1.x, pos1.y, pos2.x, pos2.y);
    SDL_SetRenderDrawColor(pRenderWindow->pRenderer, 0, 0, 0, 255);
    return;
}

SDL_Renderer *windowGetRenderer(RenderWindow const *pRenderWindow) {
    return pRenderWindow->pRenderer;
}

int windowGetWidth(RenderWindow const *pRenderWindow) {
    return pRenderWindow->width;
}

int windowGetHeight(RenderWindow const *pRenderWindow) {
    return pRenderWindow->height;
}

void destroyRenderWindow(RenderWindow *pRenderWindow) {
    if (pRenderWindow == NULL) { return; }
    
    SDL_DestroyWindow(pRenderWindow->pWindow);
    SDL_DestroyRenderer(pRenderWindow->pRenderer);
    return;
}
