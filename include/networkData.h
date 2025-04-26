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
    PlayerState state;
    int tick;
} StateData;

typedef struct {
    Vec2 input;
    int tick;
} InputData;

typedef struct {
    StateData players[MAX_PLAYERS];
    int playerID;
    ServerState serverState;
} ServerPayload;

typedef struct {
    InputData player;
    int playerID;
    ClientState clientState;
} ClientPayload;