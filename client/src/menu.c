#include "menu.h"

#define MAX_MENU_BUTTONS 12
#define MENU_SCALER 3

struct menu {
    SDL_Texture *pMenuTexture;
    Entity logo;
    Entity menuButtons[MAX_MENU_BUTTONS];
    int nrOfButtons;
};

Menu *createMenu(SDL_Renderer *pRenderer, Camera const *pCamera) {
    Menu *pMenu = malloc(sizeof(Menu));
    pMenu->pMenuTexture = IMG_LoadTexture(pRenderer, "lib/resources/menyButtonsHJ.png");
    if (pMenu->pMenuTexture == NULL) { return NULL; }

    Vec2 position = createVector(0.0f, 0.0f);
    for (int i = 0; i < MAX_MENU_BUTTONS; i++) {
        if (entityInitData(&(pMenu->menuButtons[i]), position, ENTITY_DEFAULT, HITBOX_NONE)) { return NULL; }
    }

    position = createVector(cameraGetWidth(pCamera)*0.5f, 0.0f);
    if (entityInitData(&(pMenu->logo), position, ENTITY_DEFAULT, HITBOX_NONE)) { return NULL; }
    pMenu->logo.source.w = 48;
    pMenu->logo.source.h = 32;
    pMenu->logo.frame.w = pMenu->logo.source.w*4;
    pMenu->logo.frame.h = pMenu->logo.source.h*4;
    pMenu->logo.frame.x -= pMenu->logo.frame.w*0.5f;
    
    pMenu->nrOfButtons = 0;
    return pMenu;
}

void createMenuButtons(Menu *pMenu, SDL_Rect offsetInfo, Camera *pCamera) {
    if (pMenu->nrOfButtons > MAX_MENU_BUTTONS) { return; }

    Vec2 displayMiddle = createVector(cameraGetWidth(pCamera)*0.5f, cameraGetHeight(pCamera)*0.5f);
    int startingPoint = 0;
    for (int i = 0; i < pMenu->nrOfButtons; i++) {
        if (i == 0) { startingPoint = 160; }

        pMenu->menuButtons[i].source.x = 0 + offsetInfo.x*i;
        pMenu->menuButtons[i].source.y = 0 + offsetInfo.y*i;
        pMenu->menuButtons[i].source.w = offsetInfo.w;
        pMenu->menuButtons[i].source.h = offsetInfo.h;

        pMenu->menuButtons[i].frame.w = offsetInfo.w*MENU_SCALER;
        pMenu->menuButtons[i].frame.h = offsetInfo.h*MENU_SCALER;
        pMenu->menuButtons[i].frame.x = displayMiddle.x-pMenu->menuButtons[i].frame.w*0.5f;
        pMenu->menuButtons[i].frame.y = startingPoint-pMenu->menuButtons[i].frame.h*0.5f;
        startingPoint += offsetInfo.h*1.5f*MENU_SCALER;
    }

    return;
}

int checkButtonIntersection(Camera *pCamera, Entity *menuButtons, int nrOfButtons) {
    int x, y;
    SDL_GetMouseState(&x, &y);

    SDL_FPoint mousePosition;
    float displayScale = cameraGetDisplayScale(pCamera);
    mousePosition.x = (float)x / displayScale;
    mousePosition.y = (float)y / displayScale;

    for (int i = 0; i < nrOfButtons; i++) {
        if (SDL_PointInFRect(&mousePosition, &(menuButtons[i].frame))) {
            return i+1;
        }
    }

    return 0;
}


bool mainMenu(Menu *pMenu, RenderWindow *pWindow, char *pServerAddress) {
    SDL_Rect offsetInfo;
    offsetInfo.x = 0;
    offsetInfo.y = 16;
    offsetInfo.w = 64;
    offsetInfo.h = 16;
    pMenu->nrOfButtons = 2;
    createMenuButtons(pMenu, offsetInfo, windowGetCamera(pWindow));
    SDL_SetRelativeMouseMode(SDL_FALSE);

    windowClearFrame(pWindow);
    windowRenderEntity(pWindow, pMenu->logo, RENDER_LOGO);
    for (int i = 0; i < pMenu->nrOfButtons; i++) {
        windowRenderMenu(pWindow, pMenu->pMenuTexture, pMenu->menuButtons, pMenu->nrOfButtons);
    }
    windowDisplayFrame(pWindow);

    int currentPosition = -1;
    int hover = 0;
    Input *pInput = createInputTracker();

    bool gameRunning = true;
    bool menuRunning = true;
    while (menuRunning) {
        menuRunning = checkUserInput(pInput);
        if (!menuRunning) { gameRunning = false; }

        hover = checkButtonIntersection(windowGetCamera(pWindow), pMenu->menuButtons, pMenu->nrOfButtons);

        if ((getKeyState(pInput, KEY_H) == KEY_STATE_DOWN && currentPosition == -1) || (hover == 1 && getMouseState(pInput, MOUSE_LEFT) == KEY_STATE_DOWN)) {
            strncpy(pServerAddress, "127.0.0.1", IP_MAX_LENGTH);
            menuRunning = false;
        }
        else if ((getKeyState(pInput, KEY_J) == KEY_STATE_DOWN) || (hover == 2 && getMouseState(pInput, MOUSE_LEFT) == KEY_STATE_DOWN)) {
            currentPosition = 0;
        }

        if (currentPosition >= 0) {
            if (currentPosition < IP_MAX_LENGTH-1) {
                for (int i = KEY_0; i <= KEY_9; i++) {
                    if (getKeyState(pInput, i) == KEY_STATE_DOWN) {
                        pServerAddress[currentPosition++] = i+48;
                        pServerAddress[currentPosition] = '\0';
                    }
                }

                if (getKeyState(pInput, KEY_PERIOD) == KEY_STATE_DOWN) {
                    pServerAddress[currentPosition++] = '.';
                    pServerAddress[currentPosition] = '\0';
                }
            }

            if (getKeyState(pInput, KEY_BACKSPACE) == KEY_STATE_DOWN) {
                if (currentPosition > 0) {
                    currentPosition--;
                    pServerAddress[currentPosition] = '\0';
                }
            }

            if (getKeyState(pInput, KEY_RETURN) == KEY_STATE_DOWN) {
                if (currentPosition >= 7) {
                    pServerAddress[currentPosition] = '\0';
                    menuRunning = false;
                }
            }

            inputHoldTimer(pInput);
            windowClearFrame(pWindow);
            if (currentPosition > 0) {
                windowRenderText(pWindow, pServerAddress, 0.5f, 0.5f);
            }
            windowDisplayFrame(pWindow);
        }

        SDL_Delay(1.0f/120.0f);
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);
    destroyInputTracker(pInput);
    return gameRunning;
}

void destroyMenu(Menu *pMenu) {
    if (pMenu == NULL) { return; }

    SDL_DestroyTexture(pMenu->pMenuTexture);
    free(pMenu);
    return;
}