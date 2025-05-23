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
    int sheetFlip;
    int state;
    int tick;
} StateData;

typedef struct {
    Vec2 input;
    Vec2 mouseAim;
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

/*
Prepares the given InputData with the relevant data from the supplied Player.
*/
void prepareInputData(InputData *data, Player const *player, int tick);

/*
Prepares the given StateData with the relevant data from the supplied Player.
*/
void prepareStateData(StateData *data, Player const *player, int tick);

/*
Prepares the given EntityData with the relevant data from the supplied Entity.
*/
void prepareEntityData(EntityData *data, Entity const *entity, int entityID, int tick);