#include "server.h"

void closeServer(Server *pServer) {
    if (pServer->socket) { SDLNet_UDP_Close(pServer->socket); }
    if (pServer->pPacket) { SDLNet_FreePacket(pServer->pPacket); }
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (pServer->players[i]) { destroyPlayer(pServer->players[i]); }
    }

    if (pServer->pObjects) { destroyDynamicArray(pServer->pObjects); }

    SDLNet_Quit();
    SDL_Quit();
    return;
}

int initServer(Server *pServer) {
    pServer->nrOfPlayers = 0;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        pServer->players[i] = createPlayer(createVector(PLAYER_START_X+48*i, PLAYER_START_Y), NULL, i);
        if (pServer->players[i] == NULL) { return 1; }
    }

    pServer->pObjects = createDynamicArray(ARRAY_HITBOXES);
    if (pServer->pObjects == NULL) { return 1; }
    // Add blocks along the bottom of the screen.
    for(int i = 0; i < 1920; i+=32) {
        addHitbox(pServer->pObjects, i, 1080-32, 32, 32, HITBOX_FULL_BLOCK);
    }

    for (int i = 0; i < 6*32; i+=32) {
        addHitbox(pServer->pObjects, i, 96, 32, 32, HITBOX_FULL_BLOCK);
        addHitbox(pServer->pObjects, i, 128, 32, 32, HITBOX_FULL_BLOCK);
        addHitbox(pServer->pObjects, i, 160, 32, 32, HITBOX_FULL_BLOCK);
        addHitbox(pServer->pObjects, i+6*32, 192, 32, 32, HITBOX_FULL_BLOCK);
    }

    for (int i = 0; i < 4*32; i+=32) {
        addHitbox(pServer->pObjects, i+7*32, 96, 32, 32, HITBOX_FULL_BLOCK);
    }

    addHitbox(pServer->pObjects, 0, 32, 32, 32, HITBOX_HALF_BLOCK);
    addHitbox(pServer->pObjects, 1920-32, 32, 32, 32, HITBOX_FULL_BLOCK);

    pServer->state = SERVER_WAITING;
    if (!(pServer->socket = SDLNet_UDP_Open(SERVER_PORT))) {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return 1;
    }

    if (!(pServer->pPacket = SDLNet_AllocPacket(PACKET_SIZE))) {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return 1;
    }

    pServer->pGurka = createEntity(createVector(64.0f, 64.0f), NULL, 0, HITBOX_FULL_BLOCK);

    pServer->payload.serverState = SERVER_WAITING;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        pServer->payload.players[i].position = playerGetPosition(pServer->players[i]);
        pServer->payload.players[i].tonguePosition = tongueGetPosition(playerGetTongue(pServer->players[i]));
        pServer->payload.players[i].state = playerGetState(pServer->players[i]);
        pServer->payload.players[i].sheetPosition = createVector(0.0f, 0.0f);
        pServer->payload.players[i].tick = 0;
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        pServer->payload.entities[i].position = createVector(0.0f, 0.0f);
        pServer->payload.entities[i].entityID = -1;
        pServer->payload.entities[i].tick = 0;
    }

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
    for (int i = 0; i < pServer->nrOfPlayers; i++) {
        pServer->payload.playerID = i;
        memcpy(pServer->pPacket->data, &(pServer->payload), sizeof(ServerPayload));
        pServer->pPacket->len = sizeof(ServerPayload);
        pServer->pPacket->address = pServer->clients[i];
        SDLNet_UDP_Send(pServer->socket, -1, pServer->pPacket);
    }
    return;
}

void handleTick(Server *pServer, ClientPayload payload, float const timestep) {
    if (payload.player.tick < pServer->payload.players[payload.playerID].tick) { return; }

    int movedEntity = -1;
    Player *pPlayer = pServer->players[payload.playerID];
    playerOverrideState(pPlayer, payload.player.state);
    playerOverrideVelocity(pPlayer, payload.player.input, payload.player.rotateVelocity);
    playerUpdateAnimation(pPlayer);
    switch (payload.player.state) {
        case ROTATING:
            Vec2 rotateVelocity;
            Vec2 newRotPos = playerUpdatePosition(pPlayer, timestep);
            vectorSub(&rotateVelocity, newRotPos, entityGetMidPoint(pServer->pGurka));
            entityMove(pServer->pGurka, rotateVelocity);
            tongueCalculateShaft(playerGetTongue(pPlayer), entityGetMidPoint(playerGetBody(pPlayer)), entityGetMidPoint(pServer->pGurka));
            movedEntity = 0;
            break;
        default:
            playerUpdatePosition(pPlayer, timestep);
            break;
    }

    switch (payload.player.state) {
        case SHOOTING:
        case RELEASE:
            Vec2 velocity = payload.player.tongueInput;
            vectorScale(&velocity, timestep);
            entityMove(tongueGetTip(playerGetTongue(pPlayer)), velocity);
            if (tongueCheckCollision(playerGetTongue(pPlayer), pServer->pGurka)) {
                playerSetState(pPlayer, ROTATING);
                float radius = vectorLength(entityGetMidPoint(playerGetBody(pPlayer)), entityGetMidPoint(pServer->pGurka));
                playerSetRadius(pPlayer, radius);
            }
            movedEntity = 0;
            break;
        case ROTATING:
            tongueCalculateShaft(playerGetTongue(pPlayer), entityGetMidPoint(playerGetBody(pPlayer)), entityGetMidPoint(pServer->pGurka));
            break;
        default:
            tongueSetPosition(playerGetTongue(pPlayer), entityGetMidPoint(playerGetBody(pPlayer)));
    }

    bool standingOnPlatform = false;
    for (int i = 0; i < arrayGetSize(pServer->pObjects); i++) {
        if (playerCheckCollision(pPlayer, arrayGetObject(pServer->pObjects, i)) == OBJECT_IS_NORTH) {
            standingOnPlatform = true;
        }
    }

    if (!standingOnPlatform) { playerSetState(pPlayer, FALLING); }

    pServer->payload.players[payload.playerID].position = playerGetPosition(pPlayer);
    pServer->payload.players[payload.playerID].tonguePosition = tongueGetPosition(playerGetTongue(pPlayer));
    pServer->payload.players[payload.playerID].state = playerGetState(pPlayer);
    pServer->payload.players[payload.playerID].sheetPosition.x = playerGetSheetPosition(pPlayer).x;
    pServer->payload.players[payload.playerID].sheetPosition.y = playerGetSheetPosition(pPlayer).y;
    pServer->payload.players[payload.playerID].tick = payload.player.tick;

    pServer->payload.entities[0].position = entityGetPosition(pServer->pGurka);
    pServer->payload.entities[0].entityID = movedEntity;
    pServer->payload.entities[0].tick = 0;
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
                    sendDataToClients(&server);
                    while (SDLNet_UDP_Recv(server.socket, server.pPacket) == 1) {
                        memcpy(&clientPayload, server.pPacket->data, server.pPacket->len);
                        switch (clientPayload.clientState) {
                            case CONNECTED:
                                handleTick(&server, clientPayload, timestep);
                                break;
                            case DISCONNECTED:
                                handlePlayerDisconnect(&server, clientPayload);
                                break;
                        }
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