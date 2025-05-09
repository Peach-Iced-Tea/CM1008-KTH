#pragma once
#include <stdbool.h>

#include "vmath.h"
#include "player.h"

#define MAX_PLAYERS 2
#define PACKET_SIZE 512
#define TICK_RATE 60.0f
#define BUFFER_SIZE 1024
#define SERVER_PORT 50505

typedef enum ServerState {
    SERVER_WAITING, SERVER_RUNNING, SERVER_CLOSING
} ServerState;

typedef enum ClientState {
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
} ServerPayload;

typedef struct {
    InputData player;
    int playerID;
    ClientState clientState;
} ClientPayload;

void prepareInputData(InputData *data, Player const *player, int tick);

void prepareStateData(StateData *data, Player const *player, int tick);

void prepareEntityData(EntityData *data, Entity const *entity, int entityID, int tick);