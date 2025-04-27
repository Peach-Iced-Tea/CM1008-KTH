#pragma once
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "networkData.h"
#include "utils.h"

#define GAME_CONNECTING 0
#define GAME_RUNNING 1
#define GAME_CLOSING 2

typedef struct client Client;

Client *createClient(int port);

bool clientConnectToServer(Client *pClient, IPaddress serverAddress);

void clientDisconnectFromServer(Client *pClient);

bool clientWaitForServer(Client *pClient);

bool clientReceivePacket(Client *pClient, ServerPayload *pPayload);

void clientSendPacket(Client *pClient);

void clientAddInputToBuffer(Client *pClient, InputData input);

void clientAddStateToBuffer(Client *pClient, StateData state);

void clientHandleServerReconciliation(Client *pClient, Player *pPlayer, DynamicArray *pObjects);

int clientCheckServerPayload(Client *pClient, StateData latestServerState);

InputData clientGetLatestInput(Client const *pClient);

StateData clientGetLatestState(Client const *pClient);

int clientGetPlayerID(Client const *pClient);

void destroyClient(Client *pClient);