#include "client.h"

typedef struct DataSection
{
    int ID;
    float x;
    float y;
    float dx;
    float dy;
} dataSection;

typedef struct Client{
    int ID;
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
    memcpy(pPlayer->packet->data, &pPlayer->ID, sizeof(pPlayer->ID));
	pPlayer->packet->len = sizeof(pPlayer->ID);
    sendPacket(pPlayer);
    while (wait){
        if(SDLNet_UDP_Recv(pPlayer->clientSocket, pPlayer->packet)){
            wait = false;
            int recPacket;
            memcpy(&recPacket, pPlayer->packet->data, pPlayer->packet->len);
            pPlayer->ID = recPacket;
            printf("Recieved Package.\n");
        }
    }
    printf("Connection complete.");
    
}

void sendPacket(client *pPlayer){
    SDLNet_UDP_Send(pPlayer->clientSocket, -1, pPlayer->packet);
    printf("Packet Sent\n");
}

void recPacket(){

}