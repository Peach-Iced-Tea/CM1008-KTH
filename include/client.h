#pragma once
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "networkData.h"

#define GAME_CONNECTING 0
#define GAME_RUNNING 1
#define GAME_CLOSING 2

typedef struct client Client;

Client *createClient(int port);

void destroyClient(Client *pClient);