#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "renderWindow.h"
#include "entity.h"
#include <windows.h>
#include <pthread.h>

#define GAME_CONNECTING 0
#define GAME_RUNNING 1
#define GAME_CLOSING 2
#define CONNECTION_SETUP 0
#define CONNECTION_SERVER 1
#define CONNECTION_WAIT 2

typedef struct Client client;

void gameMenu(RenderWindow *pWindow,SDL_Event *pEvent,int *pGameState, char ipAddr[16]);
void serverConnect(char serverIP[16], int port, client *pPlayer);
void sendPacket(client *pPlayer);
void recPacket();
