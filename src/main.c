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
    destroyClient(pGame->pClient);
    SDLNet_Quit();
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
        pGame->players[i] = createPlayer(createVector(PLAYER_START_X, PLAYER_START_Y), getRenderer(pGame->pWindow), i);
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

    pGame->pClient = createClient(0);
    if (pGame->pClient == NULL) { return 1; }

    return 0;
}

void updatePlayer(Player *pPlayer, Player *pTeammate, DynamicArray *pObjects, float deltaTime) {
    switch (playerGetState(pPlayer)) {
        case ROTATING:
            Entity *pBodyP2 = playerGetBody(pTeammate);
            Vec2 velocity;
            Vec2 newRotPos = playerUpdatePosition(pPlayer, deltaTime);
            vectorSub(&velocity, newRotPos, entityGetMidPoint(pBodyP2));
            entityMove(pBodyP2, velocity);
            break;
        default:
            playerUpdatePosition(pPlayer, deltaTime);
            break;
    }

    bool standingOnPlatform = false;
    for (int i = 0; i < arrayGetSize(pObjects); i++) {
        if (playerCheckCollision(pPlayer, arrayGetObject(pObjects, i)) == OBJECT_IS_NORTH) {
            standingOnPlatform = true;
        }
    }

    if (playerCheckCollision(pPlayer, playerGetBody(pTeammate)) == OBJECT_IS_NORTH) {
        standingOnPlatform = true;
    }

    if (!standingOnPlatform) { playerSetState(pPlayer, FALLING); }
    return;
}

int main(int argv, char** args) {
    if (SDL_Init(SDL_INIT_VIDEO)!=0) {
        printf("Error: %s\n",SDL_GetError());
        return 1;
    }

    if (SDLNet_Init()) {
        SDL_Quit();
        printf("Error: %s\n", SDLNet_GetError());
        return 1;
    }

    Game game;
    if (initGame(&game)) {
        cleanUp(&game);
        return 1;
    }
 
    const float timestep = 1.0f/TICK_RATE;
    Uint64 lastTime = SDL_GetTicks64();
    float deltaTime = 0.0f;
    float accumulator = 0.0f;

    Player *pPlayer = game.players[0];
    Player *pTeammate = game.players[1];
    cameraSetRenderer(game.pCamera, getRenderer(game.pWindow));
    cameraSetTargets(game.pCamera, playerGetBody(pPlayer), playerGetBody(pTeammate));

    bool gameRunning = true;
    while (gameRunning) {
        Uint64 currentTime = SDL_GetTicks64();
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
            if (vectorLength(mousePosition, entityGetMidPoint(pBodyP1)) < 240.0f) {
                Entity *pBodyP2 = playerGetBody(pTeammate);
                if (pointVsRect(entityGetHitbox(pBodyP2), mousePosition)) {
                    playerSetState(pPlayer, ROTATING);
                    float radius = vectorLength(entityGetMidPoint(pBodyP1), entityGetMidPoint(pBodyP2));
                    float alpha = vectorGetAngle(entityGetMidPoint(pBodyP1), entityGetMidPoint(pBodyP2));
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

        updatePlayer(pPlayer, pTeammate, game.pPlatforms, deltaTime);

        cameraUpdate(game.pCamera);

        clearWindow(game.pWindow);

        renderPlayer(game.pWindow, pPlayer, game.pCamera);
        renderPlayer(game.pWindow, pTeammate, game.pCamera);
        for (int i = 0; i < arrayGetSize(game.pPlatforms); i++) {
            Entity *pEntity = arrayGetObject(game.pPlatforms, i);
            if (entityIsVisible(game.pCamera, entityGetCurrentFrame(pEntity))) {
                renderEntity(game.pWindow, pEntity, game.pCamera);
            }
        }

        if (playerGetMouseClick(pPlayer)) {
            drawLine(game.pWindow, mousePosition, entityGetMidPoint(playerGetBody(pPlayer)), game.pCamera);
        }
        
        displayWindow(game.pWindow);
    }

    cleanUp(&game);

    return 0;
}
