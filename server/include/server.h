#pragma once
#include "networkData.h"
#include "entity.h"
#include "player.h"
#include "utils.h"
#include "physics.h"
#include "platforms.h"
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
    ServerMap *pMap;
    DynamicArray *pHitforms;
    MovablePlatform *pPlatform;
} Server;