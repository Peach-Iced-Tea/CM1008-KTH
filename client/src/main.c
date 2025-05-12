#include "main.h"

void cleanUp(Game *pGame) {
    if (pGame->pInput) { destroyInputTracker(pGame->pInput); }
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (pGame->players[i]) { destroyPlayer(pGame->players[i]); }
    }

    if (pGame->pCamera) { destroyCamera(pGame->pCamera); }
    if (pGame->pWindow) { destroyRenderWindow(pGame->pWindow); }
    if (pGame->pMenu) { destroyMenu(pGame->pMenu); }
    if (pGame->pClient) { destroyClient(pGame->pClient); }
    if (pGame->pMap) { destroyMap(pGame->pMap); }         // ändrat i den
    SDLNet_Quit();
    TTF_Quit();
    SDL_Quit();
    return;
}

int initGame(Game *pGame) {
    // Get information about the main display, such as pixel width and pixel height.
    SDL_DisplayMode mainDisplay;
    SDL_GetDesktopDisplayMode(0, &mainDisplay);

    pGame->pWindow = createRenderWindow("Hopless", mainDisplay.w, mainDisplay.h);
    if (pGame->pWindow == NULL) { return 1; }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        pGame->players[i] = createPlayer(createVector(PLAYER_START_X+48*i, PLAYER_START_Y), windowGetRenderer(pGame->pWindow), i);
        if (pGame->players[i] == NULL) { return 1; }
    }

    pGame->pCamera = createCamera(mainDisplay.w, mainDisplay.h, mainDisplay.refresh_rate, SCALING);
    if (pGame->pCamera == NULL) { return 1; }

    pGame->pInput = createInputTracker();
    if (pGame->pInput == NULL) { return 1; }

    pGame->pMenu = createMenu(pGame->pWindow);
    if (pGame->pMenu == NULL) { return 1; }

    pGame->pClient = createClient(0);
    if (pGame->pClient == NULL) { return 1; }

    /////////////////////////////////////////////////////// Ny /////////////////////////////////////////////////////////////////

    loadMap(pGame); //har bytt ut mot createClientMap(windowGetRenderer(pGame->pWindow));

    if (pGame->pMap->mapSheetImage.filePath) {
        
        SDL_Surface* surface = IMG_Load(pGame->pMap->mapSheetImage.filePath);
        if (surface) {
            pGame->pMap->mapSheetImage.texture = SDL_CreateTextureFromSurface(windowGetRenderer(pGame->pWindow), surface);
            pGame->pMap->mapSheetImage.width = surface->w;
            pGame->pMap->mapSheetImage.height = surface->h;
            SDL_FreeSurface(surface);
        }
    } 

    pGame->pHitforms = createDynamicArray(ARRAY_ENTITIES);
    if (pGame->pHitforms == NULL) {return 1;}

    for (int i = 0; i < pGame->pMap->blockCount; i++) {
        Block* pBlock = &pGame->pMap->blocks[i];
        Vec2 tmp = createVector(pBlock->x + 16, pBlock->y + 16);    // det var enda settet att få bilden till hitboxen ( om någon kan fixa det skulle det vara snyggare utan offseten av halva intity storklek)
        if (arrayAddObject(pGame->pHitforms, createEntity(tmp,pGame->pMap->mapSheetImage.texture,ENTITY_BLOCK,HITBOX_FULL_BLOCK))) { return 1; }
    } 
    return 0;
}


// test funkon för att flytta horisontella och vertikala (Kan flyttas på och utvecklas så att den köter alla typer men då type av kollisioner måste läggas till)
// på något set måste vi också sätta så att platformer ska ha sina pos från varandra så att blocken inte lägger efter eller överlappar när dem rör sig annars så är statiska stabila
void updatePlatforms(Game *pGame, float timestep) {
 
    for (int i = 0; i < pGame->pMap->blockCount; i++) {
        Block *pBlock = &pGame->pMap->blocks[i];
        Hitbox *pHitbox = arrayGetObject(pGame->pHitforms, i); // Get the corresponding hitbox

        float oldX = pBlock->x;
        float oldY = pBlock->y;

        switch (pBlock->type) { 
            case HORIZONTAL:
                pBlock->x += pBlock->direction * pBlock->speed * timestep;
                if (pBlock->x >= pBlock->origX + pBlock->moveRange || pBlock->x <= pBlock->origX - pBlock->moveRange) {
                    pBlock->direction = -pBlock->direction;
                }
                break;
            case VERTICAL:
                pBlock->y += pBlock->direction * pBlock->speed * timestep;  
                if (pBlock->y >= pBlock->origY + pBlock->moveRange || pBlock->y <= pBlock->origY - pBlock->moveRange) {
                    pBlock->direction = -pBlock->direction;
                }
                break;
            case DROP:
                break;
            case LIFT:
                break;
            case SWITCH:
                break;
            
        }
        hitboxPositionAdd(pHitbox, createVector(pBlock->x - oldX, pBlock->y - oldY));
    }
}
//////////////////////////////////////////////////END///////////////////////////////////////////////////////






void updatePlayer(Player *pPlayer, Player *pTeammate, DynamicArray *pObjects, float const timestep) {
    playerUpdatePosition(pPlayer, timestep);
    switch (playerGetInfo(pPlayer).state) {
        case SHOOTING:
            if (tongueCheckCollision(playerGetTongue(pPlayer), playerGetBody(pTeammate))) {
                playerSetState(pPlayer, ROTATING);
                playerSetGrabbedEntity(pPlayer, playerGetBody(pTeammate));
            }
            break;
    }

    bool standingOnPlatform = false;
    for (int i = 0; i < arrayGetSize(pObjects); i++) {
        if (playerCheckCollision(pPlayer, arrayGetObject(pObjects, i)) == OBJECT_IS_NORTH) {
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
    windowClearFrame(pGame->pWindow);

    for (int i = 0; i < MAX_PLAYERS; i++) {
        windowRenderPlayer(pGame->pWindow, pGame->players[i], pGame->pCamera);
    }

    windowRenderMapLayer(pGame->pWindow, pGame->pMap, pGame->pCamera);   // har ändrat i den

    for (int i = 0; i < arrayGetSize(pGame->pHitforms); i++) {
        windowRenderHitbox(pGame->pWindow, arrayGetObject(pGame->pHitforms, i), pGame->pCamera);
    }
    
    windowDisplayFrame(pGame->pWindow);
    return;
}

void handleTick(Game *pGame, Player *pPlayer2) {
    Player *pPlayer = pGame->players[clientGetPlayerID(pGame->pClient)];
    playerUpdateAnimation(pPlayer);
    playerUpdateState(pPlayer);

    InputData input;
    prepareInputData(&input, pPlayer, 0);
    clientAddInputToBuffer(pGame->pClient, input);
    clientSendPacket(pGame->pClient);

    ServerPayload payload;
    while (clientReceivePacket(pGame->pClient, &payload)) {
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (payload.entities[i].entityID == -1) { continue; }

            if (payload.entities[i].entityID == 0) {
                //logic for moving Entity on both screens
            }
        }

        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (i == clientGetPlayerID(pGame->pClient)) {
                if (payload.players[i].state != LOCKED) {
                    if (playerGetInfo(pPlayer).state == LOCKED) { playerOverrideState(pPlayer, payload.players[i].state); }

                    if (clientCheckServerPayload(pGame->pClient, payload.players[i])) {
                        clientHandleServerReconciliation(pGame->pClient, pPlayer, pGame->pHitforms);
                    }
                }
                else {
                    playerOverrideState(pGame->players[i], payload.players[i].state);
                    playerSetPosition(pGame->players[i], payload.players[i].position);
                }
            }
            else {
                playerSetPosition(pGame->players[i], payload.players[i].position);
                tongueSetPosition(playerGetTongue(pGame->players[i]), payload.players[i].tonguePosition);
                tongueCalculateShaft(playerGetTongue(pGame->players[i]), entityGetMidPoint(playerGetBody(pGame->players[i])), payload.players[i].tonguePosition);
                playerOverrideState(pGame->players[i], payload.players[i].state);
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

    windowClearFrame(pGame->pWindow);
    windowRenderText(pGame->pWindow, "Connecting to server...", windowGetWidth(pGame->pWindow)*0.5f, windowGetHeight(pGame->pWindow)*0.5f);
    windowDisplayFrame(pGame->pWindow);
    gameRunning = clientConnectToServer(pGame->pClient, serverAddress);
    if (!gameRunning) { return gameRunning; }

    windowClearFrame(pGame->pWindow);
    windowRenderText(pGame->pWindow, "Waiting for other players...", windowGetWidth(pGame->pWindow)*0.5f, windowGetHeight(pGame->pWindow)*0.5f);
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
    if (initGame(&game)) {      // har ändrat i den så att den loadar mapen
        cleanUp(&game);
        return 1;
    }

    const float timestep = 1.0f/TICK_RATE;
    Uint64 lastTime = SDL_GetTicks64();
    float deltaTime = 0.0f;
    float accumulator = 0.0f;

    Player *pPlayer = game.players[0];
    Player *pTeammate = game.players[1];
    cameraSetRenderer(game.pCamera, windowGetRenderer(game.pWindow));
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
                tongueSetMousePosition(playerGetTongue(pPlayer), mousePosition);
                
                while (accumulator >= timestep) {    
                    // Add physics related calculations here...
                    inputHoldTimer(game.pInput);
                    handleTick(&game, pTeammate);
                    updatePlatforms(&game, timestep);       // ny funkon för att flytta platformer (Kan utvecklas för att hantera Switch trigade och kollikon trigade)
                    updatePlayer(pPlayer, pTeammate, game.pHitforms, timestep);
                    StateData state;
                    prepareStateData(&state, pPlayer, 0);
                    clientAddStateToBuffer(game.pClient, state);
        
                    accumulator -= timestep;
                }
        
                cameraUpdate(game.pCamera);
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
