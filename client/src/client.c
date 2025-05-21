#include "client.h"

#define MAX_WAIT_TIME 30    // This constant represents time elapsed in seconds.
#define DELAY_TICK 120

struct client {
    UDPsocket socket;
    UDPpacket *pPacket;
    ClientPayload payload;
    IPaddress serverAddress;
    StateData stateBuffer[BUFFER_SIZE];
    InputData inputBuffer[BUFFER_SIZE];
    StateData lastProcessedState;
    int currentTick;
    int lastServerTick;
};

void initClientPayload(ClientPayload *pPayload) {
    pPayload->player.input = createVector(0.0f, 0.0f);
    pPayload->player.mouseAim = createVector(0.0f, 0.0f);
    pPayload->player.rotateVelocity = 0.0f;
    pPayload->player.state = IDLE;
    pPayload->player.tick = 0;
    pPayload->playerID = 0;
    pPayload->clientState = DISCONNECTED;
    return;
}

Client *createClient(int port) {
    Client *pClient = malloc(sizeof(Client));
    initClientPayload(&(pClient->payload));
    if (!(pClient->socket = SDLNet_UDP_Open(port))) {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return NULL;
    }

    if (!(pClient->pPacket = SDLNet_AllocPacket(PACKET_SIZE))) {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return NULL;
    }

    SDLNet_ResolveHost(&(pClient->serverAddress), "127.0.0.1", SERVER_PORT);
    pClient->currentTick = 0;
    pClient->lastServerTick = -1;

    return pClient;
}

bool clientConnectToServer(Client *pClient, char *pServerAddress) {
    SDLNet_ResolveHost(&(pClient->serverAddress), pServerAddress, SERVER_PORT);
    clientSendPacket(pClient);

    Input *pInput = createInputTracker();
    int timer = 0;
    bool gameRunning = true;
    bool waitForAnswer = true;
    while (waitForAnswer) {
        gameRunning = checkUserInput(pInput);
        if (!gameRunning) { break; }

        if (timer%DELAY_TICK == DELAY_TICK-1) {
            clientSendPacket(pClient);
        }

        if (timer == DELAY_TICK*MAX_WAIT_TIME) {
            gameRunning = false;
            break;
        }

        if (SDLNet_UDP_Recv(pClient->socket, pClient->pPacket)) {
            waitForAnswer = false;
            ServerPayload serverPayload;
            memcpy(&serverPayload, pClient->pPacket->data, pClient->pPacket->len);
            pClient->payload.playerID = serverPayload.playerID;
            pClient->payload.clientState = CONNECTED;
        }

        timer++;
        SDL_Delay(1000/DELAY_TICK);
    }

    if (gameRunning) {
        printf("Connected: id=%d\n", pClient->payload.playerID);
    }

    destroyInputTracker(pInput);
    return gameRunning;
}

void clientDisconnectFromServer(Client *pClient) {
    pClient->payload.clientState = DISCONNECTED;
    clientSendPacket(pClient);
    return;
}

bool clientWaitForServer(Client *pClient) {
    Input *pInput = createInputTracker();
    int timer = 0;
    bool gameRunning = true;
    bool waitingForServer = true;
    while (waitingForServer) {
        gameRunning = checkUserInput(pInput);
        if (!gameRunning) { break; }

        if (timer == DELAY_TICK*MAX_WAIT_TIME*3) {
            gameRunning = false;
            break;
        }

        if (SDLNet_UDP_Recv(pClient->socket, pClient->pPacket)) {
            ServerPayload payload;
            memcpy(&payload, pClient->pPacket->data, pClient->pPacket->len);
            if (payload.serverState == SERVER_RUNNING) { waitingForServer = false; }
        }

        timer++;
        SDL_Delay(1000/DELAY_TICK);
    }

    destroyInputTracker(pInput);
    return gameRunning;
}

bool clientReceivePacket(Client *pClient, ServerPayload *pPayload) {
    bool receivedPacket = false;
    if (SDLNet_UDP_Recv(pClient->socket, pClient->pPacket) == 1) {
        memcpy(pPayload, pClient->pPacket->data, pClient->pPacket->len);
        if (pPayload->playerID == pClient->payload.playerID) {
            receivedPacket = true;
        }
    }

    return receivedPacket;
}

void clientSendPacket(Client *pClient) {
    int bufferIndex = pClient->currentTick % BUFFER_SIZE;
    pClient->payload.player = pClient->inputBuffer[bufferIndex];
    pClient->payload.player.tick = pClient->currentTick;

    memcpy(pClient->pPacket->data, &(pClient->payload), sizeof(ClientPayload));
    pClient->pPacket->len = sizeof(ClientPayload);
    pClient->pPacket->address = pClient->serverAddress;

    SDLNet_UDP_Send(pClient->socket, -1, pClient->pPacket);
    return;
}

void clientAddInputToBuffer(Client *pClient, InputData input) {
    int bufferIndex = pClient->currentTick % BUFFER_SIZE;
    input.tick = pClient->currentTick;
    pClient->inputBuffer[bufferIndex] = input;
    return;
}

void clientAddStateToBuffer(Client *pClient, StateData state) {
    int bufferIndex = pClient->currentTick % BUFFER_SIZE;
    state.tick = pClient->currentTick;
    pClient->stateBuffer[bufferIndex] = state;
    pClient->currentTick++;
    return;
}

bool statesAreEqual(StateData state1, StateData state2) {
    if (state1.position.x != state2.position.x) { return false; }
    if (state1.position.y != state2.position.y) { return false; }

    return true;
}

void clientHandleServerReconciliation(Client *pClient, Player *pPlayer, DynamicArray *pObjects) {
    int tickToProcess = pClient->lastProcessedState.tick+1;
    int bufferIndex = tickToProcess % BUFFER_SIZE;
    playerSetPosition(pPlayer, pClient->lastProcessedState.position);

    float timestep = 1.0f/TICK_RATE;
    while (tickToProcess < pClient->currentTick) {
        bufferIndex = tickToProcess % BUFFER_SIZE;
        Vec2 velocity = pClient->inputBuffer[bufferIndex].input;
        vectorScale(&velocity, timestep);
        playerOverrideMove(pPlayer, velocity);

        bool standingOnPlatform = false;
        for (int i = 0; i < arrayGetSize(pObjects); i++) {
            if (playerCheckCollision(pPlayer, arrayGetObject(pObjects, i), true) == OBJECT_IS_NORTH) {
                standingOnPlatform = true;
            }
        }

        if (!standingOnPlatform) { playerSetState(pPlayer, FALLING); }

        StateData state;
        prepareStateData(&state, pPlayer, tickToProcess);
        pClient->stateBuffer[bufferIndex] = state;
        tickToProcess++;
    }

    return;
}

int clientCheckServerPayload(Client *pClient, StateData latestServerState) {
    if (!statesAreEqual(pClient->lastProcessedState, latestServerState)) {
        pClient->lastProcessedState = latestServerState;
        int bufferIndex = latestServerState.tick % BUFFER_SIZE;
        float positionError = vectorLength(latestServerState.position, pClient->stateBuffer[bufferIndex].position);
        if (positionError > 0.001f) {
            pClient->stateBuffer[bufferIndex] = latestServerState;
            return 1;
        }
    }

    return 0;
}

int clientHandleTick(Client *pClient, Player *pPlayer, Player *pTeammate, DynamicArray *pHitforms) {
    InputData input;
    prepareInputData(&input, pPlayer, 0);
    clientAddInputToBuffer(pClient, input);
    clientSendPacket(pClient);

    int missingTicks = 0;
    ServerPayload payload;
    while (clientReceivePacket(pClient, &payload)) {
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (payload.entities[i].entityID == -1) { continue; }

            if (payload.entities[i].entityID == 0) {
                //logic for moving Entity on both screens
            }
        }

        if (pClient->lastServerTick < payload.serverTick) {
            missingTicks += payload.serverTick - pClient->lastServerTick;
            pClient->lastServerTick = payload.serverTick;
        }

        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (i == pClient->payload.playerID) {
                if (payload.players[i].state != LOCKED) {
                    if (playerGetInfo(pPlayer).state == LOCKED) { playerOverrideState(pPlayer, payload.players[i].state); }
                    if (payload.players[i].state == ROTATING) {
                        playerSetGrabbedEntity(pPlayer, NULL, pTeammate);
                        playerOverrideState(pPlayer, payload.players[i].state);
                    }

                    if (clientCheckServerPayload(pClient, payload.players[i])) {
                        clientHandleServerReconciliation(pClient, pPlayer, pHitforms);
                    }
                }
                else {
                    playerOverrideState(pPlayer, payload.players[i].state);
                    playerSetPosition(pPlayer, payload.players[i].position);
                }
            }
            else {
                playerSetPosition(pTeammate, payload.players[i].position);
                tongueSetPosition(playerGetTongue(pTeammate), payload.players[i].tonguePosition);
                tongueCalculateShaft(playerGetTongue(pTeammate), entityGetMidPoint(playerGetBody(pTeammate)), payload.players[i].tonguePosition);
                playerOverrideState(pTeammate, payload.players[i].state);
                playerSetSheetPosition(pTeammate, payload.players[i].sheetPosition, payload.players[i].sheetFlip);
            }
        }
    }

    return missingTicks;
}

void clientSetLastServerTick(Client *pClient, int newTick) {
    pClient->lastServerTick = newTick;
    return;
}

InputData clientGetLatestInput(Client const *pClient) {
    int bufferIndex = pClient->currentTick % BUFFER_SIZE;
    return pClient->inputBuffer[bufferIndex];
}

StateData clientGetLatestState(Client const *pClient) {
    int bufferIndex = (pClient->currentTick-1) % BUFFER_SIZE;
    return pClient->stateBuffer[bufferIndex];
}

int clientGetPlayerID(Client const *pClient) {
    return pClient->payload.playerID;
}

int clientGetLastServerTick(Client const *pClient) {
    return pClient->lastServerTick;
}

void destroyClient(Client *pClient) {
    if (pClient == NULL) { return; }

    SDLNet_FreePacket(pClient->pPacket);
    SDLNet_UDP_Close(pClient->socket);
    free(pClient);
    return;
}