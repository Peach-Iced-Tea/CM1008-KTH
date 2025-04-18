#include "client.h"
#include "vmath.h"

typedef struct DataSection
{
    int ID;
    Vec2 pos;
} dataSection;

typedef struct ReturnData{
    int playerNumber;
    dataSection players[4];
} returnData;

typedef struct Client{
    UDPpacket *packet;
    IPaddress serverAddress;
    UDPsocket clientSocket;
    dataSection data;
} client;

void gameMenu(RenderWindow *pWindow, SDL_Event *pEvent, int *pGameState, char ipAddr[16]){

    Entity *pMenuButton;
    SDL_Texture *pMenu;
    pMenu = loadTexture(pWindow, "resources/menyButtonsHJ.png");
    pMenuButton = createEntity(createVector(64, 24), pMenu, HITBOX_NONE);

    clearWindow(pWindow);
    renderMenu(pWindow, pMenuButton);
    displayWindow(pWindow);

    bool menuRunning = true;
    while(menuRunning){
        while(SDL_PollEvent(pEvent)) {  // Turn this into a function, maybe add a struct containing a Vec2* and some other variables.
            if(pEvent->type == SDL_QUIT) {
                menuRunning = false;
            }
            else if(pEvent->type == SDL_KEYDOWN) {
                switch(pEvent->key.keysym.scancode) {
                    case SDL_SCANCODE_J:
                        strcpy(ipAddr, "127.0.0.1");
                        menuRunning = false;
                        break;
                    case SDL_SCANCODE_H:
                        strcpy(ipAddr, "127.0.0.1");




                        menuRunning = false;
                        break;
                    }
                }
            }
        }
    }
    
void serverConnect(char serverIP[16], int port, client *pPlayer){
        
    bool wait = true;
    SDLNet_Init();
    pPlayer->clientSocket = SDLNet_UDP_Open(0);
    pPlayer->packet = SDLNet_AllocPacket(512);

    SDLNet_ResolveHost(&pPlayer->serverAddress, "127.0.0.1", 50505);
    pPlayer->packet->address = pPlayer->serverAddress;
    memcpy(pPlayer->packet->data, &pPlayer->data, sizeof(dataSection));
	pPlayer->packet->len = sizeof(dataSection);
    sendPacket(pPlayer);

    while (wait){
        if(SDLNet_UDP_Recv(pPlayer->clientSocket, pPlayer->packet)){
            wait = false;
            dataSection recPacket;
            memcpy(&recPacket, pPlayer->packet->data, pPlayer->packet->len);
            pPlayer->data.ID = recPacket.ID;
        }
    }
    // printf("Connection complete.");
    
}

void sendPacket(client *pPlayer){
    SDLNet_UDP_Send(pPlayer->clientSocket, -1, pPlayer->packet);
    // printf("Packet Sent\n");
}

void recPacket(client *pPlayer, returnData *playerData){
    UDPpacket *recPacket = SDLNet_AllocPacket(1024);
    if(SDLNet_UDP_Recv(pPlayer->clientSocket, recPacket)){
        memcpy(playerData, recPacket->data, recPacket->len);
        // for (int i = 0; i < playerData->playerNumber; i++){
        //     printf("player %d exists at %f,%f\n", i, playerData->players[i].pos.x, playerData->players[i].pos.x);
        // }
    }
    SDLNet_FreePacket(recPacket);
}