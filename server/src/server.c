#include "server.h"

void closeServer(Server *pServer) {
    if (pServer->socket) { SDLNet_UDP_Close(pServer->socket); }
    if (pServer->pPacket) { SDLNet_FreePacket(pServer->pPacket); }
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (pServer->players[i]) { destroyPlayer(pServer->players[i]); }
    }

    if (pServer->pHitforms) { destroyDynamicArray(pServer->pHitforms); }
    if (pServer->pCheckpoints) { destroyDynamicArray(pServer->pCheckpoints); }
    if (pServer->pObstacles) { destroyDynamicArray(pServer->pObstacles); }
    if (pServer->pPlatform) { destroyPlatform(pServer->pPlatform); }

    if (pServer->pMap) { destroyMap_Server(pServer->pMap); }

    SDLNet_Quit();
    SDL_Quit();
    return;
}

int initServer(Server *pServer) {
    pServer->nrOfPlayers = 0;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        pServer->players[i] = createPlayer(createVector(PLAYER_START_X+48*i, PLAYER_START_Y), i);
        if (pServer->players[i] == NULL) { return 1; }
        pServer->lastCheckpoint[i] = -1;
    }
    printf("Created %d player(s)\n", MAX_PLAYERS);

    pServer->pMap = createServerMap();
    if (pServer->pMap == NULL) { return 1; }
    mapLoadServer("lib/resources/mapData/map.tmj", pServer->pMap);
    printf("Initiated map\n");

    int mapWidth = mapGetWidth_Server(pServer->pMap);
    int tileSize = 32;

    pServer->pHitforms = createDynamicArray(ARRAY_HITBOXES);
    if(pServer->pHitforms == NULL) { return 1; }
    printf("Initiated DynamicArray of type (Hitbox)\n");

    Vec2 tmp;
    for (size_t i = 0; i < mapGetLayerSize_Server(pServer->pMap, 0); i++) {
        int check = mapGetLayerData_Server(pServer->pMap, 0, i);
        if (check > 0) {
            float posX = (i % mapWidth) * tileSize;
            float posY = (i / mapWidth) * tileSize;
            tmp = createVector(posX, posY);

            if (arrayAddObject(pServer->pHitforms, createHitbox(tmp, tileSize, tileSize, HITBOX_FULL_BLOCK))) { return 1; }
        }
    }
    printf("Created %ld hitboxes from the map\n", arrayGetSize(pServer->pHitforms));

    pServer->state = SERVER_WAITING;
    if (!(pServer->socket = SDLNet_UDP_Open(SERVER_PORT))) {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return 1;
    }
    printf("Opened a UDP socket on port: %d\n", SERVER_PORT);

    if (!(pServer->pPacket = SDLNet_AllocPacket(PACKET_SIZE))) {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return 1;
    }
    printf("Allocated packet with size: %d\n", PACKET_SIZE);

    pServer->payload.serverState = SERVER_WAITING;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        prepareStateData(&(pServer->payload.players[i]), pServer->players[i], 0);
    }
    printf("Prepared StateData for all players\n");

    for (int i = 0; i < MAX_PLAYERS; i++) {
        prepareEntityData(&(pServer->payload.entities[i]), NULL, -1, 0);
    }
    printf("Prepared EntityData for movable entities\n");

    pServer->pObstacles = createDynamicArray(ARRAY_OBSTACLES);
    printf("Initiated DynamicArray of type (Obstacle)\n");
    if (pServer->pObstacles == NULL) { return 1; }
    
    pServer->pCheckpoints = createDynamicArray(ARRAY_OBSTACLES);
    printf("Initiated DynamicArray of type (Obstacle)\n");
    if (pServer->pCheckpoints == NULL) { return 1; }

    for (size_t i = 0; i < mapGetLayerSize_Server(pServer->pMap, 2); i++) {
        int check = mapGetLayerData_Server(pServer->pMap, 2, i);
        if (check == 1) {
            float posX = (i % mapWidth) * tileSize;
            float posY = (i / mapWidth) * tileSize;
            tmp = createVector(posX, posY);

            if (arrayAddObject(pServer->pObstacles, createObstacle(tmp, OBSTACLE_SPIKE))) { return 1; }
        }
        else if (check == 2) {
            float posX = (i % mapWidth) * tileSize;
            float posY = (i / mapWidth) * tileSize;
            tmp = createVector(posX, posY);

            if (arrayAddObject(pServer->pCheckpoints, createObstacle(tmp, OBSTACLE_CHECKPOINT))) { return 1; }
        }
    }

    printf("Created %ld obstacles\n", arrayGetSize(pServer->pObstacles));
    printf("Created %ld checkpoints\n", arrayGetSize(pServer->pCheckpoints));

    pServer->pPlatform = createPlatform(createVector(768.0f, 2880.0f), 7, PLATFORM_CONVEX);
    if (pServer->pPlatform == NULL) { return 1; }
    printf("Created MovablePlatform\n");

    pServer->currentTick = 0;

    return 0;
}

bool addIP(IPaddress clients[], IPaddress newAddress, int *pNrOfClients) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clients[i].host == newAddress.host && clients[i].port == newAddress.port) { return false; }
    }

    printf("New client connected (id: %d)\n", *pNrOfClients);
    clients[*pNrOfClients] = newAddress;
    (*pNrOfClients)++;
    return true;
}

void sendDataToClients(Server *pServer) {
    pServer->payload.serverState = pServer->state;
    pServer->payload.serverTick = pServer->currentTick;
    for (int i = 0; i < pServer->nrOfPlayers; i++) {
        pServer->payload.playerID = i;
        memcpy(pServer->pPacket->data, &(pServer->payload), sizeof(ServerPayload));
        pServer->pPacket->len = sizeof(ServerPayload);
        pServer->pPacket->address = pServer->clients[i];
        SDLNet_UDP_Send(pServer->socket, -1, pServer->pPacket);
    }
    return;
}

void updatePlayer(Server *pServer, Player *pPlayer, Player *pTeammate, int playerID, float const timestep) {
    playerUpdatePosition(pPlayer, timestep);
    switch (playerGetInfo(pPlayer).state) {
        case SHOOTING:
            if (tongueCheckCollision(playerGetTongue(pPlayer), playerGetBody(pTeammate))) {
                playerSetState(pPlayer, ROTATING);
                playerOverrideState(pTeammate, LOCKED);
                playerSetGrabbedEntity(pPlayer, NULL, pTeammate);
            }
            break;
    }

    for (int i = 0; i < arrayGetSize(pServer->pCheckpoints); i++) {
        if (playerCheckCollision(pPlayer, obstacleGetHitbox(arrayGetObject(pServer->pCheckpoints, i)), false)) {
            pServer->lastCheckpoint[playerID] = i;
        }
    }

    for (int i = 0; i < arrayGetSize(pServer->pObstacles); i++) {
        if (playerCheckCollision(pPlayer, obstacleGetHitbox(arrayGetObject(pServer->pObstacles, i)), false)) {
            playerSetState(pPlayer, IDLE);
            Vec2 tmp = obstacleGetPosition(arrayGetObject(pServer->pCheckpoints, pServer->lastCheckpoint[playerID]));
            tmp.y -= 32;
            playerSetPosition(pPlayer, tmp);
        }
    }

    bool movingPlatform = false;
    for (int i = 0; i < platformGetSize(pServer->pPlatform); i++) {
        if (playerCheckCollision(pPlayer, platformGetHitbox(pServer->pPlatform, i), true) == OBJECT_IS_NORTH) {
            movingPlatform = true;
        }
    }

    if (movingPlatform) {
        Vec2 currentPosition = playerGetInfo(pPlayer).position;
        vectorAdd(&currentPosition, currentPosition, platformGetVelocity(pServer->pPlatform));
        playerSetPosition(pPlayer, currentPosition);
    }

    bool standingOnPlatform = false;
    for (int i = 0; i < arrayGetSize(pServer->pHitforms); i++) {
        if (playerCheckCollision(pPlayer, arrayGetObject(pServer->pHitforms, i), true) == OBJECT_IS_NORTH) {
            standingOnPlatform = true;
        }
    }

    if (!standingOnPlatform) { playerSetState(pPlayer, FALLING); }
    return;
}

void handleTick(Server *pServer, ClientPayload payload, float const timestep) {
    if (payload.player.tick <= pServer->payload.players[payload.playerID].tick) { return; }

    int movedEntity = -1;
    Player *pPlayer = pServer->players[payload.playerID];
    Player *pTeammate = pServer->players[0];
    if (payload.playerID == 0) {
        pTeammate = pServer->players[1];
    }

    switch (playerGetInfo(pPlayer).state) {
        case LOCKED:
            break;
        case ROTATING:
            if (payload.player.state == RELEASE) {
                playerOverrideState(pPlayer, payload.player.state);
                tongueSetMousePosition(playerGetTongue(pPlayer), payload.player.mouseAim);
                tongueUpdateVelocity(playerGetTongue(pPlayer), playerGetMidPoint(pPlayer));
            }
            break;
        default:
            if (payload.player.state != ROTATING) {
                playerOverrideState(pPlayer, payload.player.state);
            }
            
            if (payload.player.state == SHOOTING && tongueGetInfo(playerGetTongue(pPlayer)).state == NEUTRAL) {
                tongueSetMousePosition(playerGetTongue(pPlayer), payload.player.mouseAim);
                tongueUpdateVelocity(playerGetTongue(pPlayer), playerGetMidPoint(pPlayer));
            }
            else if (payload.player.state == RELEASE && tongueGetInfo(playerGetTongue(pPlayer)).state == EXTENDING) {
                tongueUpdateVelocity(playerGetTongue(pPlayer), playerGetMidPoint(pPlayer));
            }
    }

    playerOverrideVelocity(pPlayer, payload.player.input, payload.player.rotateVelocity);
    playerUpdateAnimation(pPlayer);
    updatePlayer(pServer, pPlayer, pTeammate, payload.playerID, timestep);

    prepareStateData(&(pServer->payload.players[payload.playerID]), pPlayer, payload.player.tick);
    prepareEntityData(&(pServer->payload.entities[payload.playerID]), NULL, movedEntity, 0);
    return;
}

void handlePlayerDisconnect(Server *pServer, ClientPayload payload) {
    if (payload.playerID < pServer->nrOfPlayers-1) {
        for (int i = payload.playerID; i < pServer->nrOfPlayers-1; i++) {
            pServer->clients[i] = pServer->clients[i+1];
        }
    }

    printf("Client disconnected (id: %d)\n", payload.playerID);
    pServer->nrOfPlayers--;
    return;
}

int main(int argv, char** args) {
    if (SDL_Init(SDL_INIT_EVENTS)!=0) {
        printf("Error: %s\n",SDL_GetError());
        return 1;
    }

    if (SDLNet_Init()) {
        SDL_Quit();
        printf("Error: %s\n", SDLNet_GetError());
        return 1;
    }

    Server server;
    if (initServer(&server)) {
        closeServer(&server);
        return 1;
    }

    const float delay = 1.0f/120.0f;
    const float timestep = 1.0f/TICK_RATE;
    Uint64 lastTime = SDL_GetTicks64();
    float deltaTime = 0.0f;
    float accumulator = 0.0f;

    ClientPayload clientPayload;
    SDL_Event event;

    bool serverRunning = true;
    printf("Waiting for players\n");
    while (serverRunning) {
        Uint64 currentTime = SDL_GetTicks64();
        deltaTime = (currentTime - lastTime) * 0.001f; // ms till sekunder
        lastTime = currentTime;

        switch (server.state) {
            case SERVER_WAITING:
                if (SDL_PollEvent(&event)) { if (event.type == SDL_QUIT) { server.state = SERVER_CLOSING; } }

                if (SDLNet_UDP_Recv(server.socket, server.pPacket) == 1) {
                    if (addIP(server.clients, server.pPacket->address, &(server.nrOfPlayers))) {
                        server.payload.playerID = server.nrOfPlayers-1;
                        memcpy(server.pPacket->data, &(server.payload), sizeof(ServerPayload));
                        server.pPacket->len = sizeof(ServerPayload);
                        SDLNet_UDP_Send(server.socket, -1, server.pPacket);
                    }
                }

                if (server.nrOfPlayers == MAX_PLAYERS) {
                    printf("Starting\n");
                    server.state = SERVER_RUNNING;
                    sendDataToClients(&server);
                }
                break;
            case SERVER_RUNNING:
                accumulator += deltaTime;
                if (SDL_PollEvent(&event)) { if (event.type == SDL_QUIT) { server.state = SERVER_CLOSING; } }

                while (accumulator >= timestep) {
                    for (int i = 0; i < MAX_PLAYERS; i++) {
                        prepareStateData(&(server.payload.players[i]), server.players[i], -1);
                    }
                    
                    platformMove(server.pPlatform);
                    sendDataToClients(&server);
                    while (SDLNet_UDP_Recv(server.socket, server.pPacket) == 1) {
                        memcpy(&clientPayload, server.pPacket->data, server.pPacket->len);
                        switch (clientPayload.clientState) {
                            case CONNECTED:
                                handleTick(&server, clientPayload, timestep);
                                break;
                            case DISCONNECTED:
                                if (clientPayload.playerID != -1) {
                                    handlePlayerDisconnect(&server, clientPayload);
                                }
                                break;
                        }
                    }

                    for (int i = 0; i < MAX_PLAYERS; i++) {
                        switch (playerGetInfo(server.players[i]).state) {
                            case ROTATING:
                                break;
                            default:
                                int teammateID = 1;
                                if (i == 1) { teammateID = 0; }
                                switch (playerGetInfo(server.players[teammateID]).state) {
                                    case LOCKED:
                                        playerOverrideState(server.players[teammateID], IDLE);
                                        server.payload.players[teammateID].state = playerGetInfo(server.players[teammateID]).state;
                                        break;
                                }
                        }

                        server.payload.players[i].state = playerGetInfo(server.players[i]).state;
                    }

                    server.currentTick++;
                    accumulator -= timestep;
                }

                if (server.nrOfPlayers == 0) { server.state = SERVER_CLOSING; }
                break;
            case SERVER_CLOSING:
                closeServer(&server);
                printf("Server Closed!\n");
                serverRunning = false;
                break;
        }

        SDL_Delay(delay);
    }

    return 0;
}