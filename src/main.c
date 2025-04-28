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
    destroyMenu(pGame->pMenu);
    destroyClient(pGame->pClient);
    SDLNet_Quit();
    TTF_Quit();
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
        pGame->players[i] = createPlayer(createVector(PLAYER_START_X+48*i, PLAYER_START_Y), getRenderer(pGame->pWindow), i);
        if (pGame->players[i] == NULL) { return 1; }
    }

    pGame->pGurkaTexture = loadTexture(pGame->pWindow, "resources/player1.png");

    pGame->pGurka = createEntity(createVector(64.0f, 64.0f), pGame->pGurkaTexture, 0, HITBOX_FULL_BLOCK);


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

    pGame->pMenu = createMenu(pGame->pWindow);
    if (pGame->pMenu == NULL) { return 1; }

    pGame->pClient = createClient(0);
    if (pGame->pClient == NULL) { return 1; }

    return 0;
}

void updatePlayer(Player *pPlayer, Player *pTeammate, DynamicArray *pObjects, float deltaTime, Game *pGame) {
    switch (playerGetState(pPlayer)) {
        case ROTATING:
            Vec2 rotateVelocity;
            Vec2 newRotPos = playerUpdatePosition(pPlayer, deltaTime);
            vectorSub(&rotateVelocity, newRotPos, entityGetMidPoint(pGame->pGurka));
            entityMove(pGame->pGurka, rotateVelocity);
            vectorMidPoint(&newRotPos, entityGetMidPoint(playerGetBody(pPlayer)), entityGetMidPoint(pGame->pGurka));
            tongueSetShaftMidPoint(playerGetTongue(pPlayer), newRotPos);

            break;
        default:
            playerUpdatePosition(pPlayer, deltaTime);
            break;
    }

    bool standingOnPlatform = false;
    for (int i = 0; i < arrayGetSize(pObjects); i++) {
        if (playerCheckCollision(pPlayer, entityGetHitbox(arrayGetObject(pObjects, i))) == OBJECT_IS_NORTH) {
            standingOnPlatform = true;
        }
    }

    /*if (playerCheckCollision(pPlayer, playerGetBodyHitbox(pTeammate)) == OBJECT_IS_NORTH) {
        standingOnPlatform = true;
    }*/

    if (!standingOnPlatform) { playerSetState(pPlayer, FALLING); }
    return;
}

void updateDisplay(Game *pGame, Vec2 mousePosition) {
    Player *pPlayer = pGame->players[clientGetPlayerID(pGame->pClient)];
    clearWindow(pGame->pWindow);

    for (int i = 0; i < MAX_PLAYERS; i++) {
        renderPlayer(pGame->pWindow, pGame->players[i], pGame->pCamera);
    }
    //-------------------------------TONGUE RENDER----------------------------------
    renderEntity(pGame->pWindow, pGame->pGurka, pGame->pCamera);

    for (int i = 0; i < arrayGetSize(pGame->pPlatforms); i++) {
        Entity *pEntity = arrayGetObject(pGame->pPlatforms, i);
        if (entityIsVisible(pGame->pCamera, entityGetCurrentFrame(pEntity))) {
            renderEntity(pGame->pWindow, pEntity, pGame->pCamera);
        }
    }

    /*switch (playerGetState(pPlayer)) {
        case SHOOTING:
        case RELEASE:
            drawLine(pGame->pWindow, entityGetMidPoint(tongueGetTip(playerGetTongue(pPlayer))), entityGetMidPoint(playerGetBody(pPlayer)), pGame->pCamera);
            break;
        case ROTATING:
            drawLine(pGame->pWindow, entityGetMidPoint(playerGetBody(pPlayer)), entityGetMidPoint(pGame->pGurka), pGame->pCamera);
            break;
    }*/
    
    displayWindow(pGame->pWindow);
    return;
}

void handleTick(Game *pGame) {
    Player *pPlayer = pGame->players[clientGetPlayerID(pGame->pClient)];
    playerUpdateState(pPlayer);

    InputData input;
    input.input = playerGetVelocity(pPlayer);
    input.sheetPosition.x = playerGetSheetPosition(pPlayer).x;
    input.sheetPosition.y = playerGetSheetPosition(pPlayer).y;
    clientAddInputToBuffer(pGame->pClient, input);
    clientSendPacket(pGame->pClient);

    ServerPayload payload;
    while (clientReceivePacket(pGame->pClient, &payload)) {
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (i == clientGetPlayerID(pGame->pClient)) {
                if(clientCheckServerPayload(pGame->pClient, payload.players[i])) {
                    clientHandleServerReconciliation(pGame->pClient, pPlayer, pGame->pPlatforms);
                }
            }
            else {
                entitySetPosition(playerGetBody(pGame->players[i]), payload.players[i].position);
                playerSetSheetPosition(pGame->players[i], payload.players[i].sheetPosition);
            }
        }
    }

    return;
}

bool initiateConnection(Game *pGame) {
    bool gameRunning = true;
    IPaddress serverAddress;
    gameRunning = mainMenu(pGame->pMenu, pGame->pWindow, &serverAddress);
    if (!gameRunning) { return gameRunning; }

    clearWindow(pGame->pWindow);
    renderText(pGame->pWindow, "Connecting to server...", windowGetWidth(pGame->pWindow)*0.5f, windowGetHeight(pGame->pWindow)*0.5f);
    displayWindow(pGame->pWindow);
    gameRunning = clientConnectToServer(pGame->pClient, serverAddress);
    if (!gameRunning) { return gameRunning; }

    clearWindow(pGame->pWindow);
    renderText(pGame->pWindow, "Waiting for other players...", windowGetWidth(pGame->pWindow)*0.5f, windowGetHeight(pGame->pWindow)*0.5f);
    displayWindow(pGame->pWindow);
    gameRunning = clientWaitForServer(pGame->pClient);
    if (!gameRunning) { return gameRunning; }

    return gameRunning;
}

int main(int argv, char** args) {
    if (SDL_Init(SDL_INIT_VIDEO)!=0) {
        printf("Error: %s\n",SDL_GetError());
        return 1;
    }

    if (TTF_Init()) {
        SDL_Quit();
        printf("Error: %s\n", TTF_GetError());
        return 1;
    }

    if (SDLNet_Init()) {
        TTF_Quit();
        SDL_Quit();
        printf("Error: %s\n", SDLNet_GetError());
        return 1;
    }

    Game game;
    if (initGame(&game)) {
        cleanUp(&game);
        return 1;
    }
 
    Vec2 ref;
    float alpha;

    const float timestep = 1.0f/TICK_RATE;
    Uint64 lastTime = SDL_GetTicks64();
    float deltaTime = 0.0f;
    float accumulator = 0.0f;

    Player *pPlayer = game.players[0];
    Player *pTeammate = game.players[1];
    cameraSetRenderer(game.pCamera, getRenderer(game.pWindow));
    cameraSetTargets(game.pCamera, playerGetBody(pPlayer), playerGetBody(pTeammate));

    int gameState = GAME_CONNECTING;
    bool gameRunning = true;
    while (gameRunning) {
        Uint64 currentTime = SDL_GetTicks64();
        deltaTime = (currentTime - lastTime) * 0.001f; // ms till sekunder
        lastTime = currentTime;

        switch (gameState) {
            case GAME_CONNECTING:
                gameRunning = initiateConnection(&game);
                if (!gameRunning) { continue; }

                pPlayer = game.players[clientGetPlayerID(game.pClient)];
                if (clientGetPlayerID(game.pClient) == 0) {
                    pTeammate = game.players[1];
                }
                else if (clientGetPlayerID(game.pClient) == 1) {
                    pTeammate = game.players[0];
                }

                cameraSetTargets(game.pCamera, playerGetBody(pPlayer), playerGetBody(pTeammate));
                lastTime = SDL_GetTicks64();
                gameState = GAME_RUNNING;
                break;
            case GAME_RUNNING:
                accumulator += deltaTime;
        
                Vec2 mousePosition = cameraGetMousePosition(game.pCamera);
        
                if (checkUserInput(game.pInput) == 0) {
                    gameState = GAME_CLOSING;
                    continue;
                }

                playerHandleInput(pPlayer, game.pInput);
                cameraHandleInput(game.pCamera, game.pInput);
                windowHandleInput(game.pWindow, game.pInput);
   
                //----------------TONGUE and ROTATION Logic------------------------------------------------
                tongueSetMousePosition(playerGetTongue(pPlayer), mousePosition);

                switch (playerGetState(pPlayer)) {
                    case SHOOTING:
                        if (checkCollision(entityGetHitbox(game.pGurka), tongueGetHitbox(playerGetTongue(pPlayer)))) {
                            playerSetState(pPlayer, ROTATING);
                            float radius = vectorLength(entityGetMidPoint(playerGetBody(pPlayer)), entityGetMidPoint(game.pGurka));
                            playerSetRadius(pPlayer, radius);
                        }
                        break;
                    case ROTATING:
                        tongueSetPosition(playerGetTongue(pPlayer), entityGetMidPoint(game.pGurka));
                        break;
                }
                

                while (accumulator >= timestep) {    
                    // Add physics related calculations here...
                    inputHoldTimer(game.pInput);
                    handleTick(&game);
                    
                    updatePlayer(pPlayer, pTeammate, game.pPlatforms, timestep, &game);
                    cameraUpdate(game.pCamera);
                    StateData state;
                    state.position = playerGetPosition(pPlayer);
                    state.sheetPosition.x = playerGetSheetPosition(pPlayer).x;
                    state.sheetPosition.y = playerGetSheetPosition(pPlayer).y;
                    clientAddStateToBuffer(game.pClient, state);
        
                    accumulator -= timestep;
                }
        
                updateDisplay(&game, mousePosition);
                
                break;
            case GAME_CLOSING:
                clientDisconnectFromServer(game.pClient);
                printf("Disconnected from server!\n");
                gameRunning = false;
                break;
        }
    }

    cleanUp(&game);
    printf("Game closed!\n");

    return 0;
}
