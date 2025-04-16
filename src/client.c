#include "client.h"

void gameMenu(RenderWindow *pWindow, SDL_Event *pEvent, int *pGameState){

    Entity *pMenuButton;
    SDL_Texture *pMenu;
    pMenu = loadTexture(pWindow, "resources/menyButtonsHJ.png");
    pMenuButton = createEntity(createVector(64, 24), pMenu, HITBOX_NONE);

    clearWindow(pWindow);
    renderMenu(pWindow, pMenuButton);
    displayWindow(pWindow);

    bool menuRunning = true;
    while(menuRunning){
        while(SDL_PollEvent(pEvent)) {  // Turn this into a function, maybe add a struct containing a Vec2* and some other variables.
            if(pEvent->type == SDL_QUIT) {
                menuRunning = false;
            }
            else if(pEvent->type == SDL_KEYDOWN) {
                switch(pEvent->key.keysym.scancode) {
                    case SDL_SCANCODE_J:
                        menuRunning = false;
                        break;
                    case SDL_SCANCODE_H:
                        




                        menuRunning = false;
                        break;
                }
            }
        }
    }
    
    //If host -> start server
#ifdef _WIN32
    HANDLE thread = CreateThread(NULL, 0, server_WIN, NULL, 0, NULL); //Create a separate thread for server (only WIN systems)
#else
    pthread_t serverThread;
    pthread_create(&serverThread, NULL, server_POSIX, NULL); //Starts process for POSIX systems
#endif
    //Start client (always)



}