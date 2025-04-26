#include "main.h"

void cleanUp(Game *pGame) {
    destroyInputTracker(pGame->pInput);
    for (int i = 0; i < MAX_PLAYERS; i++) {
        destroyPlayer(pGame->players[i]);
    }

    destroyDynamicArray(pGame->pPlatforms);
    SDL_DestroyTexture(pGame->pGrassTexture);
    destroyCamera(pGame->pCamera);
    destroyRenderWindow(pGame->pWindow);
    SDL_Quit();
    return;
}

int initGame(Game *pGame) {
    // Get information about the main display, such as pixel width and pixel height.
    SDL_DisplayMode mainDisplay;
    SDL_GetDesktopDisplayMode(0, &mainDisplay);

    pGame->pWindow = createRenderWindow("ToTheTop", mainDisplay.w, mainDisplay.h);
    if (pGame->pWindow == NULL) { return 1; }

    pGame->pGrassTexture = loadTexture(pGame->pWindow, "resources/purpg.png");
    if (pGame->pGrassTexture == NULL) { return 1; }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        pGame->players[i] = createPlayer(createVector(48.0f, 48.0f), getRenderer(pGame->pWindow), i);
        if (pGame->players[i] == NULL) { return 1; }
    }

    pGame->pPlatforms = createDynamicArray(ARRAY_ENTITIES);
    if (pGame->pPlatforms == NULL) { return 1; }
    // Add blocks along the bottom of the screen.
    for(int i = 0; i < REFERENCE_WIDTH; i+=32) {
        addEntity(pGame->pPlatforms, i, REFERENCE_HEIGHT-32, pGame->pGrassTexture, HITBOX_FULL_BLOCK);
    }

    for (int i = 0; i < 6*32; i+=32) {
        addEntity(pGame->pPlatforms, i, 96, pGame->pGrassTexture, HITBOX_FULL_BLOCK);
        addEntity(pGame->pPlatforms, i, 128, pGame->pGrassTexture, HITBOX_FULL_BLOCK);
        addEntity(pGame->pPlatforms, i, 160, pGame->pGrassTexture, HITBOX_FULL_BLOCK);
        addEntity(pGame->pPlatforms, i+6*32, 192, pGame->pGrassTexture, HITBOX_FULL_BLOCK);
    }

    for (int i = 0; i < 4*32; i+=32) {
        addEntity(pGame->pPlatforms, i+7*32, 96, pGame->pGrassTexture, HITBOX_FULL_BLOCK);
    }

    addEntity(pGame->pPlatforms, 0, 32, pGame->pGrassTexture, HITBOX_HALF_BLOCK);
    addEntity(pGame->pPlatforms, REFERENCE_WIDTH-32, 32, pGame->pGrassTexture, HITBOX_FULL_BLOCK);

    pGame->pCamera = createCamera(mainDisplay.w, mainDisplay.h, mainDisplay.refresh_rate, SCALING);
    if (pGame->pCamera == NULL) { return 1; }

    pGame->pInput = createInputTracker();
    if (pGame->pInput == NULL) { return 1; }

    return 0;
}

int main(int argv, char** args) {
    if (SDL_Init(SDL_INIT_VIDEO)!=0) {
        printf("Error: %s\n",SDL_GetError());
        return 1;
    }

    Game game;
    if (initGame(&game)) {
        cleanUp(&game);
        return 1;
    }

    cameraSetRenderer(game.pCamera, getRenderer(game.pWindow));
 
    const float timestep = 1.0f/60.0f; // Fixed timestep (60 Updates per second)
    Uint32 lastTime = SDL_GetTicks();
    float deltaTime = 0.0f;
    float accumulator = 0.0f;

    Player *pPlayer = game.players[0];
    Player *pTeammate = game.players[1];
    cameraSetTargets(game.pCamera, playerGetBody(pPlayer), playerGetBody(pTeammate));

    bool gameRunning = true;
    while (gameRunning) {
        Uint32 currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) * 0.001f; // ms till sekunder
        lastTime = currentTime;
        accumulator += deltaTime;

        Vec2 mousePosition = cameraGetMousePosition(game.pCamera);

        gameRunning = checkUserInput(game.pInput);
        playerHandleInput(pPlayer, game.pInput);
        cameraHandleInput(game.pCamera, game.pInput);
        windowHandleInput(game.pWindow, game.pInput);

        if (playerGetMouseClick(pPlayer)) {
            Entity *pBodyP1 = playerGetBody(pPlayer);
            if (vectorLength(mousePosition, getMidPoint(pBodyP1)) < 240.0f) {
                Entity *pBodyP2 = playerGetBody(pTeammate);
                if (pointVsRect(getHitbox(pBodyP2), mousePosition)) {
                    playerSetState(pPlayer, ROTATING);
                    float radius = vectorLength(getMidPoint(pBodyP1), getMidPoint(pBodyP2));
                    float alpha = vectorGetAngle(getMidPoint(pBodyP1), getMidPoint(pBodyP2));
                    playerSetRadius(pPlayer, radius);
                    playerSetReferenceAngle(pPlayer, alpha);
                }
            }
        }

        while (accumulator >= timestep) {    
            // Add physics related calculations here...
            inputHoldTimer(game.pInput);
            playerUpdateState(pPlayer);
            accumulator -= timestep;
        }

        switch (playerGetState(pPlayer)) {
            case ROTATING:
                Entity *pBodyP2 = playerGetBody(pTeammate);
                Vec2 velocity;
                Vec2 newRotPos = playerUpdatePosition(pPlayer, deltaTime);
                vectorSub(&velocity, newRotPos, getMidPoint(pBodyP2));
                entityMove(pBodyP2, velocity);
                break;
            default:
                playerUpdatePosition(pPlayer, deltaTime);
                break;
        }

        bool standingOnPlatform = false;
        for (int i = 0; i < arrayGetSize(game.pPlatforms); i++) {
            if (playerCheckCollision(pPlayer, arrayGetObject(game.pPlatforms, i)) == OBJECT_IS_NORTH) {
                standingOnPlatform = true;
            }
        }

        if (playerCheckCollision(pPlayer, playerGetBody(pTeammate)) == OBJECT_IS_NORTH) {
            standingOnPlatform = true;
        }

        if (!standingOnPlatform) { playerSetState(pPlayer, FALLING); }

        cameraUpdate(game.pCamera);

        clearWindow(game.pWindow);

        renderPlayer(game.pWindow, pPlayer, game.pCamera);
        renderPlayer(game.pWindow, pTeammate, game.pCamera);
        for (int i = 0; i < arrayGetSize(game.pPlatforms); i++) {
            Entity *pEntity = arrayGetObject(game.pPlatforms, i);
            if (entityIsVisible(game.pCamera, getCurrentFrame(pEntity))) {
                renderEntity(game.pWindow, pEntity, game.pCamera);
            }
        }

        if (playerGetMouseClick(pPlayer)) {
            drawLine(game.pWindow, mousePosition, getMidPoint(playerGetBody(pPlayer)), game.pCamera);
        }
        
        displayWindow(game.pWindow);
    }

    cleanUp(&game);

    return 0;
}
