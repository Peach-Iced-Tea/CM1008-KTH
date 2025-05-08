#pragma once
#include <SDL2/SDL_net.h>
#include <stdbool.h>

#include "networkData.h"
#include "utils.h"

#define GAME_CONNECTING 0
#define GAME_RUNNING 1
#define GAME_CLOSING 2

typedef struct client Client;

Client *createClient(int port);

bool clientConnectToServer(Client *client, IPaddress serverAddress);

void clientDisconnectFromServer(Client *client);

bool clientWaitForServer(Client *client);

bool clientReceivePacket(Client *client, ServerPayload *payload);

void clientSendPacket(Client *client);

void clientAddInputToBuffer(Client *client, InputData input);

void clientAddStateToBuffer(Client *client, StateData state);

void clientHandleServerReconciliation(Client *client, Player *player, DynamicArray *objects);

int clientCheckServerPayload(Client *client, StateData latestServerState);

InputData clientGetLatestInput(Client const *client);

StateData clientGetLatestState(Client const *client);

int clientGetPlayerID(Client const *client);

void destroyClient(Client *client);