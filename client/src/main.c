#include "main.h"

void cleanUp(Game *pGame) {
    if (pGame->pInput) { destroyInputTracker(pGame->pInput); }
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (pGame->players[i]) { destroyPlayer(pGame->players[i]); }
    }

    if (pGame->pMouse) { destroyMouse(pGame->pMouse); }
    if (pGame->pWindow) { destroyRenderWindow(pGame->pWindow); }
    if (pGame->pHitforms) { destroyDynamicArray(pGame->pHitforms); }
    if (pGame->pCheckpoints) { destroyDynamicArray(pGame->pCheckpoints); }
    if (pGame->pObstacles) { destroyDynamicArray(pGame->pObstacles); }
    if (pGame->pPlatform) { destroyPlatform(pGame->pPlatform); }
    if (pGame->pMenu) { destroyMenu(pGame->pMenu); }
    if (pGame->pClient) { destroyClient(pGame->pClient); }
    if (pGame->pMap) { destroyMap(pGame->pMap); }
    SDLNet_Quit();
    TTF_Quit();
    SDL_Quit();
    return;
}

int initGame(Game *pGame) {
    pGame->pWindow = createRenderWindow("Hoppless");
    if (pGame->pWindow == NULL) { return 1; }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        pGame->players[i] = createPlayer(createVector(PLAYER_START_X+48*i, PLAYER_START_Y), i);
        if (pGame->players[i] == NULL) { return 1; }
    }

    pGame->pInput = createInputTracker();
    if (pGame->pInput == NULL) { return 1; }

    pGame->pMenu = createMenu(windowGetRenderer(pGame->pWindow), windowGetCamera(pGame->pWindow));
    if (pGame->pMenu == NULL) { return 1; }

    pGame->pClient = createClient(0);
    if (pGame->pClient == NULL) { return 1; }

    pGame->pMap = createMap();
    if (pGame->pMap == NULL) { return 1; }

    if (mapLoadDataFromFile(pGame->pMap, MAP_DEMO)) { return 1; }
    if (windowLoadMapTileset(pGame->pWindow, mapGetTilesetPath(pGame->pMap))) { return 1; }

    pGame->pHitforms = createDynamicArray(ARRAY_HITBOXES);
    if (pGame->pHitforms == NULL) { return 1; }

    pGame->pObstacles = createDynamicArray(ARRAY_OBSTACLES);
    if (pGame->pObstacles == NULL) { return 1; }

    pGame->pCheckpoints = createDynamicArray(ARRAY_OBSTACLES);
    if (pGame->pCheckpoints == NULL) { return 1; }

    Vec2 position;
    for (int layer = 0; layer < mapGetLayerCount(pGame->pMap); layer++) {
        for (int i = 0; i < mapGetLayerSize(pGame->pMap, layer); i++) {
            int check = mapGetLayerData(pGame->pMap, layer, i, &position);
            switch (layer) {
                case LAYER_HITBOXES:
                    int tileWidth = mapGetTileWidth(pGame->pMap);
                    int tileHeight = mapGetTileHeight(pGame->pMap);
                    if (check > 0) {
                        if (arrayAddObject(pGame->pHitforms, createHitbox(position, tileWidth, tileHeight, HITBOX_FULL_BLOCK))) { return 1; }
                    }
                    break;
                case LAYER_OBSTACLES:
                    if (check == 1) {
                        if (arrayAddObject(pGame->pObstacles, createObstacle(position, OBSTACLE_SPIKE))) { return 1; }
                    }
                    else if (check == 2) {
                        if (arrayAddObject(pGame->pCheckpoints, createObstacle(position, OBSTACLE_CHECKPOINT))) { return 1; }
                    }
                    else if (check == 3) {
                        if (entityInitData(&(pGame->end), position, ENTITY_END, HITBOX_END)) { return 1; }
                    }
                    break;
            }
        }
    }

    pGame->pPlatform = createPlatform(createVector(768.0f, 2880.0f), 5, PLATFORM_FLAT);
    if (pGame->pPlatform == NULL) { return 1; }

    pGame->pMouse = createMouse(playerGetMidPoint(pGame->players[0]));
    if (pGame->pMouse == NULL) { return 1; }

    pGame->lastCheckpoint = -1;

    return 0;
}

void updatePlayer(Game *pGame, Player *pPlayer, Player *pTeammate, float const timestep) {
    playerUpdatePosition(pPlayer, timestep);
    switch (playerGetInfo(pPlayer).state) {
        case SHOOTING:
            tongueCheckCollision(playerGetTongue(pPlayer), playerGetBody(pTeammate));
            break;
    }

    for (int i = 0; i < arrayGetSize(pGame->pCheckpoints); i++) {
        if (playerCheckCollision(pPlayer, obstacleGetHitbox(arrayGetObject(pGame->pCheckpoints, i)), false)) {
            pGame->lastCheckpoint = i;
        }
    }

    for (int i = 0; i < arrayGetSize(pGame->pObstacles); i++) {
        if (playerCheckCollision(pPlayer, obstacleGetHitbox(arrayGetObject(pGame->pObstacles, i)), false)) {
            if (obstacleIsHazardous(arrayGetObject(pGame->pObstacles, i)) == HAZARD_LETHAL) {
                playerSetState(pPlayer, IDLE);
                if (pGame->lastCheckpoint == -1) {
                    Vec2 position = createVector(PLAYER_START_X, PLAYER_START_Y);
                    playerSetPosition(pPlayer, position);
                }
                else {
                    Vec2 position = obstacleGetPosition(arrayGetObject(pGame->pCheckpoints, pGame->lastCheckpoint));
                    position.y -= playerGetBody(pPlayer).frame.h*0.5f;
                    playerSetPosition(pPlayer, position);
                }
            }
        }
    }

    bool movingPlatform = false;
    for (int i = 0; i < platformGetSize(pGame->pPlatform); i++) {
        if (playerCheckCollision(pPlayer, platformGetHitbox(pGame->pPlatform, i), true) == OBJECT_IS_NORTH) {
            movingPlatform = true;
        }
    }

    if (movingPlatform) {
        Vec2 currentPosition = playerGetInfo(pPlayer).position;
        vectorAdd(&currentPosition, currentPosition, platformGetVelocity(pGame->pPlatform));
        playerSetPosition(pPlayer, currentPosition);
    }

    bool standingOnPlatform = movingPlatform;
    for (int i = 0; i < arrayGetSize(pGame->pHitforms); i++) {
        if (playerCheckCollision(pPlayer, arrayGetObject(pGame->pHitforms, i), true) == OBJECT_IS_NORTH) {
            standingOnPlatform = true;
        }
    }

    if (!standingOnPlatform) { playerSetState(pPlayer, FALLING); }
    return;
}

void updateDisplay(Game *pGame, Vec2 mousePosition) {
    Player *pPlayer = pGame->players[clientGetPlayerID(pGame->pClient)];
    windowClearFrame(pGame->pWindow);

    Entity entity;
    entityInitData(&entity, createVector(0.0f, 0.0f), ENTITY_DEFAULT, HITBOX_NONE);
    for (int i = 0; i < mapGetLayerSize(pGame->pMap, LAYER_TILE_TEXTURES); i++) {
        if (mapGetTileInfo(pGame->pMap, i, &entity)) {
            windowRenderEntity(pGame->pWindow, entity, RENDER_MAP);
        }
    }

    for (int i = 0; i < arrayGetSize(pGame->pCheckpoints); i++) {
        Entity body = obstacleGetBody(arrayGetObject(pGame->pCheckpoints, i));
        if (i == pGame->lastCheckpoint) { body.source.x += 32; }

        windowRenderEntity(pGame->pWindow, body, RENDER_OBSTACLE);
    }

    for (int i = 0; i < arrayGetSize(pGame->pObstacles); i++) {
        windowRenderEntity(pGame->pWindow, obstacleGetBody(arrayGetObject(pGame->pObstacles, i)), RENDER_OBSTACLE);
    }

    for (int i = 0; i < platformGetSize(pGame->pPlatform); i++) {
        windowRenderEntity(pGame->pWindow, platformGetBody(pGame->pPlatform, i), RENDER_PLATFORM);
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        switch (playerGetInfo(pGame->players[i]).state) {
            case SHOOTING:
            case RELEASE:
            case ROTATING:
                windowRenderEntity(pGame->pWindow, tongueGetShaft(playerGetTongue(pGame->players[i])), RENDER_TONGUE);
                windowRenderEntity(pGame->pWindow, tongueGetTip(playerGetTongue(pGame->players[i])), RENDER_TONGUE);
                break;
        }
        windowRenderEntity(pGame->pWindow, playerGetBody(pGame->players[i]), RENDER_PLAYER1+i);
    }
    
    for (int i = 0; i < arrayGetSize(pGame->pHitforms); i++) {
        windowRenderHitbox(pGame->pWindow, arrayGetObject(pGame->pHitforms, i));
    }

    windowRenderEntity(pGame->pWindow, mouseGetBody(pGame->pMouse), RENDER_MOUSE);
    windowDisplayFrame(pGame->pWindow);
    return;
}

bool initiateConnection(Game *pGame) {
    bool gameRunning = true;
    char serverAddress[IP_MAX_LENGTH];
    gameRunning = mainMenu(pGame->pMenu, pGame->pWindow, serverAddress);
    if (!gameRunning) { return gameRunning; }

    windowClearFrame(pGame->pWindow);
    windowRenderText(pGame->pWindow, "Connecting to server...", 0.5f, 0.5f);
    windowDisplayFrame(pGame->pWindow);
    gameRunning = clientConnectToServer(pGame->pClient, serverAddress);
    if (!gameRunning) { return gameRunning; }

    windowClearFrame(pGame->pWindow);
    windowRenderText(pGame->pWindow, "Waiting for other players...", 0.5f, 0.5f);
    windowDisplayFrame(pGame->pWindow);
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

    const float timestep = 1.0f/TICK_RATE;
    Uint64 lastTime = SDL_GetTicks64();
    float deltaTime = 0.0f;
    float accumulator = 0.0f;

    Player *pPlayer = game.players[0];
    Player *pTeammate = game.players[1];
    cameraSetMapSize(windowGetCamera(game.pWindow), mapGetSize(game.pMap));

    bool endScreenIsDisplayed = false;
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

                lastTime = SDL_GetTicks64();
                gameState = GAME_RUNNING;
                break;
            case GAME_RUNNING:
                accumulator += deltaTime;
                if (checkUserInput(game.pInput) == 0) {
                    gameState = GAME_CLOSING;
                    continue;
                }
                if(playerCheckCollision(pPlayer, game.end.pHitbox, false)) {
                    gameState = GAME_WON;
                    continue;
                }

                playerHandleInput(pPlayer, game.pInput);
                cameraHandleInput(windowGetCamera(game.pWindow), game.pInput);
                windowHandleInput(game.pWindow, game.pInput);
                mouseHandleInput(game.pMouse, game.pInput);
                mouseUpdatePosition(game.pMouse, cameraGetPosition(windowGetCamera(game.pWindow)));

                Vec2 mousePosition = mouseGetPosition(game.pMouse);
                tongueSetMousePosition(playerGetTongue(pPlayer), mousePosition);
                switch (playerGetInfo(pPlayer).state) {
                    case ROTATING:
                        float targetAngle = vectorGetAngle(playerGetMidPoint(pPlayer), mouseGetPosition(game.pMouse));
                        playerCalculateRotation(pPlayer, targetAngle);
                        break;
                }
                
                while (accumulator >= timestep) {
                    // Add physics related calculations here...
                    inputHoldTimer(game.pInput);
                    playerUpdateAnimation(pPlayer);
                    playerUpdateState(pPlayer);
                    int missingTicks = clientHandleTick(game.pClient, pPlayer, pTeammate, game.pHitforms);
                    for (int i = 0; i < missingTicks; i++) { platformMove(game.pPlatform); }

                    updatePlayer(&game, pPlayer, pTeammate, timestep);
                    StateData state;
                    prepareStateData(&state, pPlayer, 0);
                    clientAddStateToBuffer(game.pClient, state);
        
                    accumulator -= timestep;
                }
        
                cameraUpdate(windowGetCamera(game.pWindow), playerGetBody(pPlayer), playerGetBody(pTeammate));
                mouseSetBorders(game.pMouse, (float)cameraGetWidth(windowGetCamera(game.pWindow)), (float)cameraGetHeight(windowGetCamera(game.pWindow)));
                updateDisplay(&game, mousePosition);
                
                break;
            case GAME_WON:
                if (checkUserInput(game.pInput) == 0) {
                    gameState = GAME_CLOSING;
                    continue;
                }
                
                if (!endScreenIsDisplayed) {
                    windowRenderText(game.pWindow, "Game Won!", 0.5f, 0.45f);
                    windowRenderText(game.pWindow, "ESC to exit", 0.5f, 0.55f);
                    windowDisplayFrame(game.pWindow);
                    endScreenIsDisplayed = true;
                }
                
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
