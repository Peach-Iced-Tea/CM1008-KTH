#pragma once
#include "networkData.h"
#include "entity.h"
#include "player.h"
#include "utils.h"
#include "physics.h"

typedef struct {
    int nrOfPlayers;
    Player *players[MAX_PLAYERS];
    DynamicArray *pObjects;
    ServerPayload payload;
    ServerState state;
    UDPsocket socket;
    UDPpacket *pPacket;
    IPaddress clients[MAX_PLAYERS];
    int currentTick;
} Server;