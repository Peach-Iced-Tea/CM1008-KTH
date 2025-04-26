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
        pServer->players[i] = createPlayer(createVector(PLAYER_START_X, PLAYER_START_Y), NULL, i);
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

    pServer->payload.serverState = SERVER_WAITING;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        pServer->payload.players[i].position = playerGetPosition(pServer->players[i]);
        pServer->payload.players[i].state = playerGetState(pServer->players[i]);
        pServer->payload.players[i].tick = 0;
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
        printf("player%d: x=%f, y=%f\n", i, pServer->payload.players[i].position.x, pServer->payload.players[i].position.y);
        memcpy(pServer->pPacket->data, &(pServer->payload), sizeof(ServerPayload));
        pServer->pPacket->len = sizeof(ServerPayload);
        pServer->pPacket->address = pServer->clients[i];
        SDLNet_UDP_Send(pServer->socket, -1, pServer->pPacket);
    }
    return;
}

void handleTick(Server *pServer, ClientPayload payload, float const timestep) {
    if (payload.player.tick < pServer->payload.players[payload.playerID].tick) { return; }

    printf("tick: %d || player%d: x=%f, y=%f\n", payload.player.tick, payload.playerID, payload.player.input.x, payload.player.input.y);
    Player *pPlayer = pServer->players[payload.playerID];
    Vec2 velocity = payload.player.input;
    vectorScale(&velocity, timestep);
    entityMove(playerGetBody(pPlayer), velocity);

    bool standingOnPlatform = false;
    for (int i = 0; i < arrayGetSize(pServer->pObjects); i++) {
        if (playerCheckCollision(pPlayer, arrayGetObject(pServer->pObjects, i)) == OBJECT_IS_NORTH) {
            standingOnPlatform = true;
        }
    }

    if (!standingOnPlatform) { playerSetState(pPlayer, FALLING); }

    pServer->payload.players[payload.playerID].position = playerGetPosition(pPlayer);
    pServer->payload.players[payload.playerID].state = playerGetState(pPlayer);
    pServer->payload.players[payload.playerID].tick = payload.player.tick;
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
                        printf("input\n");
                        memcpy(&clientPayload, server.pPacket->data, server.pPacket->len);
                        switch (clientPayload.clientState) {
                            case CONNECTED:
                                handleTick(&server, clientPayload, timestep);
                                break;
                            case DISCONNECTED:
                                if (clientPayload.playerID < server.nrOfPlayers-1) {
                                    for (int i = clientPayload.playerID; i < server.nrOfPlayers-1; i++) {
                                        server.clients[i] = server.clients[i+1];
                                    }
                                }

                                server.nrOfPlayers--;
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