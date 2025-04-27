#include "client.h"

#define IP_MAX_LENGTH 16

struct client {
    UDPsocket socket;
    UDPpacket *pPacket;
    ClientPayload payload;
    IPaddress serverAddress;
    StateData stateBuffer[BUFFER_SIZE];
    InputData inputBuffer[BUFFER_SIZE];
    StateData lastProcessedState;
    int currentTick;
};

void initClientPayload(ClientPayload *pPayload) {
    pPayload->player.input = createVector(0.0f, 0.0f);
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

    SDLNet_ResolveHost(&(pClient->serverAddress), "127.0.0.1", 50505);
    pClient->currentTick = 0;

    return pClient;
}

void clientConnectToServer(Client *pClient) {
    pClient->pPacket->address = pClient->serverAddress;
    memcpy(pClient->pPacket->data, &(pClient->payload), sizeof(ClientPayload));
    pClient->pPacket->len = sizeof(ClientPayload);
    clientSendPacket(pClient);

    bool waitForAnswer = true;
    while (waitForAnswer) {
        if (SDLNet_UDP_Recv(pClient->socket, pClient->pPacket)) {
            waitForAnswer = false;
            ServerPayload serverPayload;
            memcpy(&serverPayload, pClient->pPacket->data, pClient->pPacket->len);
            pClient->payload.playerID = serverPayload.playerID;
            pClient->payload.clientState = CONNECTED;
        }
    }

    printf("Connected: id=%d\n", pClient->payload.playerID);
    return;
}

void clientWaitForServer(Client *pClient) {
    bool waitingForServer = true;
    while (waitingForServer) {
        if (SDLNet_UDP_Recv(pClient->socket, pClient->pPacket)) {
            ServerPayload payload;
            memcpy(&payload, pClient->pPacket->data, pClient->pPacket->len);
            if (payload.serverState == SERVER_RUNNING) { waitingForServer = false; }
        }
    }
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
        entityMove(playerGetBody(pPlayer), velocity);

        bool standingOnPlatform = false;
        for (int i = 0; i < arrayGetSize(pObjects); i++) {
            if (playerCheckCollision(pPlayer, arrayGetObject(pObjects, i)) == OBJECT_IS_NORTH) {
                standingOnPlatform = true;
            }
        }

        if (!standingOnPlatform) { playerSetState(pPlayer, FALLING); }

        StateData state;
        state.position = playerGetPosition(pPlayer);
        state.sheetPosition.x = playerGetSheetPosition(pPlayer).x;
        state.sheetPosition.y = playerGetSheetPosition(pPlayer).y;
        state.tick = tickToProcess;
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

void destroyClient(Client *pClient) {
    if (pClient == NULL) { return; }

    SDLNet_FreePacket(pClient->pPacket);
    SDLNet_UDP_Close(pClient->socket);
    free(pClient);
    return;
}