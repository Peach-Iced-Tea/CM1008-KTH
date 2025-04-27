#include "menu.h"

#define IP_MAX_LENGTH 16

struct menu {
    SDL_Texture *pMainMenuTexture;
    SDL_FRect menuFrame;
    int currentEntry;
};

Menu *createMenu(RenderWindow *pWindow) {
    Menu *pMenu = malloc(sizeof(Menu));
    pMenu->pMainMenuTexture = loadTexture(pWindow, "resources/menyButtonsHJ.png");
    if (pMenu->pMainMenuTexture == NULL) { return NULL; }

    pMenu->menuFrame.x = 0.0f;
    pMenu->menuFrame.y = 0.0f;
    pMenu->menuFrame.w = 32.0f;
    pMenu->menuFrame.h = 32.0f;
    
    pMenu->currentEntry = 0;
    return pMenu;
}

bool mainMenu(Menu *pMenu, RenderWindow *pWindow, IPaddress *pServerAddress) {
    SDL_DisplayMode mainDisplay;
    SDL_GetDesktopDisplayMode(0, &mainDisplay);
    pMenu->menuFrame.x = mainDisplay.w*0.5f;
    pMenu->menuFrame.y = mainDisplay.h*0.5f;

    clearWindow(pWindow);
    renderMenu(pWindow, pMenu->menuFrame, pMenu->pMainMenuTexture);
    displayWindow(pWindow);
    int connectionState = 0;
    int currentPosition = -1;
    char ipAddress[IP_MAX_LENGTH];
    Input *pInput = createInputTracker();

    bool gameRunning = true;
    bool menuRunning = true;
    while (menuRunning) {
        menuRunning = checkUserInput(pInput);
        if (!menuRunning) { gameRunning = false; }

        if (getKeyState(pInput, KEY_H) == KEY_STATE_DOWN && currentPosition == -1) {
            SDLNet_ResolveHost(pServerAddress, "127.0.0.1", SERVER_PORT);
            menuRunning = false;
        }
        else if (getKeyState(pInput, KEY_J) == KEY_STATE_DOWN) {
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
                renderText(pWindow, ipAddress, 192, 192);
            }
            displayWindow(pWindow);
        }
    }

    return gameRunning;
}

void destroyMenu(Menu *pMenu) {
    if (pMenu == NULL) { return; }

    SDL_DestroyTexture(pMenu->pMainMenuTexture);
    free(pMenu);
    return;
}