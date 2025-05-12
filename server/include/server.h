#pragma once
#include "networkData.h"
#include "entity.h"
#include "player.h"
#include "utils.h"
#include "physics.h"

#include "mapParser.h"

typedef struct {
    int nrOfPlayers;
    Player *players[MAX_PLAYERS];
    ServerPayload payload;
    ServerState state;
    UDPsocket socket;
    UDPpacket *pPacket;
    IPaddress clients[MAX_PLAYERS];
    int currentTick;
    Map *pMap;      // har ändrat till detta
    DynamicArray *pHitforms;
} Server;