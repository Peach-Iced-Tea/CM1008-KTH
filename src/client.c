#include "client.h"

#define IP_MAX_LENGTH 16

struct client {
    UDPsocket socket;
    UDPpacket *pPacket;
    ClientPayload payload;
    IPaddress serverAddress;
    StateData stateBuffer[BUFFER_SIZE];
    InputData inputBuffer[BUFFER_SIZE];
    int currentTick;
};

void initClientPayload(ClientPayload *pPayload) {
    pPayload->player.input = createVector(0.0f, 0.0f);
    pPayload->player.tick = 0;
    pPayload->playerID = -1;
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

    pClient->currentTick = 0;

    return pClient;
}

void destroyClient(Client *pClient) {
    if (pClient == NULL) { return; }

    SDLNet_FreePacket(pClient->pPacket);
    SDLNet_UDP_Close(pClient->socket);
    free(pClient);
    return;
}