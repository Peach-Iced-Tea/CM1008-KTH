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

/*
Sends a packet to the server with the address given inside serverAddress.

Waits for a response from the server for a set amount of time before timing out.
*/
bool clientConnectToServer(Client *client, IPaddress serverAddress);

/*
Sends a packet to the server telling it that the server is going to disconnect.
*/
void clientDisconnectFromServer(Client *client);

/*
Waits for the server to send back a packet telling the clients that the server has started.
*/
bool clientWaitForServer(Client *client);

/*
Checks if a packet has been received on the UDP socket and copies it into the payload.
*/
bool clientReceivePacket(Client *client, ServerPayload *payload);

/*
Sends the latest input data that is inside the input buffer at the current tick.
*/
void clientSendPacket(Client *client);

/*
Add an InputData to the state buffer at the current tick.
*/
void clientAddInputToBuffer(Client *client, InputData input);

/*
Add a StateData to the state buffer at the current tick.
*/
void clientAddStateToBuffer(Client *client, StateData state);

/*
Calling this function will rewind player inputs from the last processed server state, up to the current input.
*/
void clientHandleServerReconciliation(Client *client, Player *player, DynamicArray *objects);

/*
Check the given StateData to see if it differs from the prediction made by the client.
*/
int clientCheckServerPayload(Client *client, StateData latestServerState);

/*

*/
void clientSetLastServerTick(Client *client, int newTick);

/*
Get the latest InputData saved inside the input buffer.
*/
InputData clientGetLatestInput(Client const *client);

/*
Get the latest StateData saved inside the state buffer.
*/
StateData clientGetLatestState(Client const *client);

/*
Get the current ID of the Client.
*/
int clientGetPlayerID(Client const *client);

/*

*/
int clientGetLastServerTick(Client const *client);

/*
Use this function to destroy the given Client pointer and free up memory.
*/
void destroyClient(Client *client);