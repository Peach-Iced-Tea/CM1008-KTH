#include "menu.h"

#define IP_MAX_LENGTH 16
#define MAX_MENU_BUTTONS 12
#define MENU_SCALER 10

struct menu {
    SDL_Texture *pMenuTexture;
    SDL_Rect menuButtons[MAX_MENU_BUTTONS];
    SDL_Rect menuPositions[MAX_MENU_BUTTONS];
    int nrOfButtons;
};

Menu *createMenu(RenderWindow *pWindow) {
    Menu *pMenu = malloc(sizeof(Menu));
    pMenu->pMenuTexture = loadTexture(pWindow, "resources/menyButtonsHJ.png");
    if (pMenu->pMenuTexture == NULL) { return NULL; }

    for (int i = 0; i < MAX_MENU_BUTTONS; i++) {
        pMenu->menuButtons[i].x = 0;
        pMenu->menuButtons[i].y = 0;
        pMenu->menuButtons[i].w = 0;
        pMenu->menuButtons[i].h = 0;

        pMenu->menuPositions[i].x = 0;
        pMenu->menuPositions[i].y = 0;
        pMenu->menuPositions[i].w = 0;
        pMenu->menuPositions[i].h = 0;
    }
    
    pMenu->nrOfButtons = 0;
    return pMenu;
}

void createMenuButtons(Menu *pMenu, SDL_Rect offsetInfo, Vec2 displayMiddle, RenderWindow *pWindow) {
    if (pMenu->nrOfButtons > MAX_MENU_BUTTONS) { return; }

    float extraScale = (float)windowGetWidth(pWindow)/REFERENCE_WIDTH;
    float extraScaleH = (float)windowGetHeight(pWindow)/REFERENCE_HEIGHT;
    if (extraScaleH < extraScale) {
        extraScale = extraScaleH;
    }

    int startingPoint = 0;
    for (int i = 0; i < pMenu->nrOfButtons; i++) {
        if (i == 0) {
            startingPoint = displayMiddle.y / pMenu->nrOfButtons;
        }

        pMenu->menuButtons[i].x = 0 + offsetInfo.x*i;
        pMenu->menuButtons[i].y = 0 + offsetInfo.y*i;
        pMenu->menuButtons[i].w = offsetInfo.w;
        pMenu->menuButtons[i].h = offsetInfo.h;

        pMenu->menuPositions[i].w = offsetInfo.w*MENU_SCALER*extraScale;
        pMenu->menuPositions[i].h = offsetInfo.h*MENU_SCALER*extraScale;
        pMenu->menuPositions[i].x = displayMiddle.x-pMenu->menuPositions[i].w*0.5f;
        pMenu->menuPositions[i].y = startingPoint+(startingPoint*2*i)-pMenu->menuPositions[i].h*0.5f;
    }

    return;
}

int checkButtonIntersection(SDL_Rect menuPosition[], int nrOfButtons) {
    SDL_Point mousePosition;
    SDL_GetMouseState(&(mousePosition.x), &(mousePosition.y));

    for (int i = 0; i < nrOfButtons; i++) {
        if (SDL_PointInRect(&mousePosition, &(menuPosition[i]))) {
            return i+1;
        }
    }

    return 0;
}


bool mainMenu(Menu *pMenu, RenderWindow *pWindow, IPaddress *pServerAddress) {
    Vec2 displayMiddle = createVector(windowGetWidth(pWindow)*0.5f, windowGetHeight(pWindow)*0.5f);
    SDL_Rect offsetInfo;
    offsetInfo.x = 0;
    offsetInfo.y = 16;
    offsetInfo.w = 64;
    offsetInfo.h = 16;
    pMenu->nrOfButtons = 2;
    createMenuButtons(pMenu, offsetInfo, displayMiddle, pWindow);

    clearWindow(pWindow);
    for (int i = 0; i < pMenu->nrOfButtons; i++) {
        renderMenu(pWindow, pMenu->pMenuTexture, pMenu->menuButtons, pMenu->menuPositions, pMenu->nrOfButtons);
    }
    displayWindow(pWindow);
    
    int connectionState = 0;
    int currentPosition = -1;
    int hover = 0;
    char ipAddress[IP_MAX_LENGTH];
    Input *pInput = createInputTracker();
    SDL_Point mouseState;

    bool gameRunning = true;
    bool menuRunning = true;
    while (menuRunning) {
        menuRunning = checkUserInput(pInput);
        if (!menuRunning) { gameRunning = false; }

        hover = checkButtonIntersection(pMenu->menuPositions, pMenu->nrOfButtons);

        if ((getKeyState(pInput, KEY_H) == KEY_STATE_DOWN && currentPosition == -1) || (hover == 1 && getMouseState(pInput, MOUSE_LEFT) == KEY_STATE_DOWN)) {
            SDLNet_ResolveHost(pServerAddress, "127.0.0.1", SERVER_PORT);
            menuRunning = false;
        }
        else if ((getKeyState(pInput, KEY_J) == KEY_STATE_DOWN) || (hover == 2 && getMouseState(pInput, MOUSE_LEFT) == KEY_STATE_DOWN)) {
            currentPosition = 0;
        }

        if (currentPosition >= 0) {
            if (currentPosition < IP_MAX_LENGTH-1) {
                for (int i = KEY_0; i < KEY_9; i++) {
                    if (getKeyState(pInput, i) == KEY_STATE_DOWN) {
                        ipAddress[currentPosition++] = i+48;
                        ipAddress[currentPosition] = '\0';
                    }
                }

                if (getKeyState(pInput, KEY_PERIOD) == KEY_STATE_DOWN) {
                    ipAddress[currentPosition++] = '.';
                    ipAddress[currentPosition] = '\0';
                }
            }

            if (getKeyState(pInput, KEY_BACKSPACE) == KEY_STATE_DOWN) {
                if (currentPosition > 0) {
                    currentPosition--;
                    ipAddress[currentPosition] = '\0';
                }
            }

            if (getKeyState(pInput, KEY_RETURN) == KEY_STATE_DOWN) {
                if (currentPosition >= 7) {
                    ipAddress[currentPosition] = '\0';
                    SDLNet_ResolveHost(pServerAddress, ipAddress, SERVER_PORT);
                    menuRunning = false;
                }
            }

            inputHoldTimer(pInput);
            clearWindow(pWindow);
            if (currentPosition > 0) {
                renderText(pWindow, ipAddress, displayMiddle.x, displayMiddle.y);
            }
            displayWindow(pWindow);
        }
    }

    return gameRunning;
}

void destroyMenu(Menu *pMenu) {
    if (pMenu == NULL) { return; }

    SDL_DestroyTexture(pMenu->pMenuTexture);
    free(pMenu);
    return;
}