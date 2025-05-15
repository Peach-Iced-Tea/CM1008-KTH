#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdbool.h>

#include "player.h"
#include "vmath.h"

#define MAX_PLAYERS 2
#define PACKET_SIZE 512
#define PLAYER_START_X 48.0f
#define PLAYER_START_Y 48.0f
#define TICK_RATE 60.0f
#define BUFFER_SIZE 1024
#define SERVER_PORT 50505

typedef enum {
    SERVER_WAITING, SERVER_RUNNING, SERVER_CLOSING
} ServerState;

typedef enum {
    CONNECTED, DISCONNECTED
} ClientState;

typedef struct {
    Vec2 position;
    Vec2 tonguePosition;
    Vec2 sheetPosition;
    int state;
    int tick;
} StateData;

typedef struct {
    Vec2 input;
    Vec2 tongueInput;
    float rotateVelocity;
    int state;
    int tick;
} InputData;

typedef struct {
    Vec2 position;
    int entityID;
    int tick;
} EntityData;

typedef struct {
    StateData players[MAX_PLAYERS];
    EntityData entities[MAX_PLAYERS];
    int playerID;
    ServerState serverState;
    int serverTick;
} ServerPayload;

typedef struct {
    InputData player;
    int playerID;
    ClientState clientState;
} ClientPayload;

void prepareInputData(InputData *pData, Player *pPlayer, int tick);

void prepareStateData(StateData *pSData, Player *pPlayer, int tick);

void prepareEntityData(EntityData *pEData, Entity *pEntity, int entityID, int tick);