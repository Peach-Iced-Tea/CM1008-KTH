#include "renderWindow.h"

typedef enum {
    WINDOWED, FULLSCREEN, EXCLUSIVE, ALT_TABBED
} WindowState;

typedef struct textures {
    SDL_Texture *pLogo;
    SDL_Texture *pPlayer1;
    SDL_Texture *pPlayer2;
    SDL_Texture *pTongue;
    SDL_Texture *pMouse;
    SDL_Texture *pObstacles;
    SDL_Texture *pPlatform;
    SDL_Texture *pMapTileset;
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
    pTextures->pLogo = IMG_LoadTexture(pRenderer, "lib/resources/logo.png");
    if (pTextures->pLogo == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
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
    pTextures->pMouse = IMG_LoadTexture(pRenderer, "lib/resources/mouse.png");
    if (pTextures->pMouse == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
    pTextures->pObstacles = IMG_LoadTexture(pRenderer, "lib/resources/obstacles.png");
    if (pTextures->pObstacles == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
    pTextures->pPlatform = IMG_LoadTexture(pRenderer, "lib/resources/movingPlatform.png");
    if (pTextures->pPlatform == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
    pTextures->pMapTileset = NULL;

    return 0;
}

RenderWindow *createRenderWindow(const char* pTitle, int w, int h) {
    RenderWindow *pWindow = malloc(sizeof(RenderWindow));
    pWindow->pWindow = SDL_CreateWindow(pTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!pWindow->pWindow) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }

    pWindow->pRenderer = SDL_CreateRenderer(pWindow->pWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if (!pWindow->pRenderer) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }

    pWindow->pFont = TTF_OpenFont("lib/resources/arial.ttf", 800);
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

int windowLoadMapTileset(RenderWindow *pWindow, char const *pFilePath) {
    pWindow->textures.pMapTileset = IMG_LoadTexture(pWindow->pRenderer, pFilePath);
    if (pWindow->textures.pMapTileset == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    return 0;
}

void toggleFullscreen(RenderWindow *pWindow) {
    switch (pWindow->state) {
        case WINDOWED:
            SDL_SetWindowFullscreen(pWindow->pWindow, 0);
            SDL_SetWindowBordered(pWindow->pWindow, SDL_TRUE);
            SDL_SetWindowSize(pWindow->pWindow, 1280, 720);
            SDL_SetWindowPosition(pWindow->pWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            SDL_SetRelativeMouseMode(SDL_FALSE);
            printf("Window State: Windowed\n");
            break;
        case FULLSCREEN:
            SDL_SetWindowFullscreen(pWindow->pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
            SDL_SetWindowSize(pWindow->pWindow, pWindow->width, pWindow->height);
            SDL_SetRelativeMouseMode(SDL_TRUE);
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
        case RENDER_LOGO:
            return textures.pLogo;
        case RENDER_PLAYER1:
            return textures.pPlayer1;
        case RENDER_PLAYER2:
            return textures.pPlayer2;
        case RENDER_TONGUE:
            return textures.pTongue;
        case RENDER_MOUSE:
            return textures.pMouse;
        case RENDER_OBSTACLE:
            return textures.pObstacles;
        case RENDER_PLATFORM:
            return textures.pPlatform;
        case RENDER_MAP:
            return textures.pMapTileset;
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

void windowRenderMenu(RenderWindow const *pWindow, SDL_Texture *pTexture, Entity const *menuButtons, int nrOfButtons) {
    for (int i = 0; i < nrOfButtons; i++) {
        SDL_Rect src = menuButtons[i].source;
        SDL_FRect dst = menuButtons[i].frame;

        SDL_RenderCopyF(pWindow->pRenderer, pTexture, &src, &dst);
    }
    
    return;
}

void windowRenderText(RenderWindow *pWindow, char const textToRender[], float widthModifier, float heightModifier) {
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
    dst.w *= 0.05f;
    dst.h *= 0.05f;
    dst.x = cameraGetWidth(pWindow->pCamera)*widthModifier-dst.w*0.5f;
    dst.y = cameraGetHeight(pWindow->pCamera)*heightModifier-dst.h*0.5f;
    SDL_RenderCopy(pWindow->pRenderer, pTexture, NULL, &dst);
    return;
}

void windowRenderEntity(RenderWindow *pWindow, Entity const entity, RenderType renderType) {
    if (pWindow->pCamera == NULL) {
        printf("Error: RenderWindow is missing Camera\n");
        return;
    }

    SDL_FRect dst = entity.frame;
    SDL_Rect src = entity.source;
    switch (renderType) {
        case RENDER_LOGO:
            break;
        default:
            if (!cameraEntityIsVisible(pWindow->pCamera, dst)) { return; }
            cameraAdjustToViewport(pWindow->pCamera, &dst, NULL);
    }

    SDL_Texture *pTexture = windowGetTexture(pWindow->textures, renderType);
    if (pTexture == NULL) { return; }

    SDL_RenderCopyExF(pWindow->pRenderer, pTexture, &src, &dst, entity.angle, NULL, entity.flip);
    if (pWindow->renderHitboxes) { windowRenderHitbox(pWindow, entity.pHitbox); }
    pWindow->nrOfRenderedEntites++;
    return;
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

Camera *windowGetCamera(RenderWindow const *pWindow) {
    return pWindow->pCamera;
}

void destroyRenderWindow(RenderWindow *pWindow) {
    if (pWindow == NULL) { return; }
    
    SDL_DestroyWindow(pWindow->pWindow);
    SDL_DestroyRenderer(pWindow->pRenderer);
    free(pWindow);
    return;
}
