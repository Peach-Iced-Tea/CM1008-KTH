#pragma once
#include <SDL2/SDL_net.h>

#include "vmath.h"
#include "entity.h"
#include "networkData.h"
#include "player.h"
#include "utils.h"
#include "mapParser.h"
#include "platform.h"

typedef struct {
    int nrOfPlayers;
    Player *players[MAX_PLAYERS];
    ServerPayload payload;
    ServerState state;
    UDPsocket socket;
    UDPpacket *pPacket;
    IPaddress clients[MAX_PLAYERS];
    int currentTick;
    Map *pMap;
    DynamicArray *pHitforms;
    DynamicArray *pCheckpoints;
    DynamicArray *pObstacles;
    MovablePlatform *pPlatform;
    int lastCheckpoint[MAX_PLAYERS];
} Server;