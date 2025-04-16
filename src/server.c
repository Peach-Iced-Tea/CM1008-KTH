#include "server.h"
#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct Message {
    char text[32+1];
} message;

#ifdef _WIN32
#include <windows.h>
DWORD WINAPI server_thread(LPVOID lpParam);
HANDLE thread;
#else
#include <pthread.h>
void* server_thread(void* param);
pthread_t thread;
#endif

volatile bool keppAlive = true;

#ifdef _WIN32
DWORD WINAPI server_thread(LPVOID param){
#else
void* server_thread(void* param){
#endif
    IPaddress ip;
    UDPsocket server;
    UDPpacket *packet;
    volatile bool keepAlive = true;

    server = SDLNet_UDP_Open(50505);
    if (!server){
        printf("Socket failed to open on port 50505");

        return 0;
    }

    packet = SDLNet_AllocPacket(512);
    if (!packet){
        printf("Packet allocation failed");
        SDLNet_UDP_Close(server);
        return 0;
    }

    printf("Server listening");
    
    while(keepAlive){
        if(SDLNet_UDP_Recv(server, packet)){
            message recieve;
            memcpy(&recieve, packet->data, packet->len);
            printf("Client sent: %s", recieve.text);
        }
    }
#ifdef _WIN32
}
#else
}
#endif

int SDL_main(int argc, char* argv[]){
    if(SDLNet_Init() < 0){
        printf("SDLNet did not init.");
    }
#ifdef _WIN32
    thread = CreateThread(NULL, 0, server_thread, NULL, 0, NULL);
#else
    pthread_create(&thread, NULL, server_thread, NULL);
#endif
}


/*
SDLNet_Init();
    UDPsocket serverSocket = SDLNet_UDP_Open(50505);
    UDPpacket *sendPacket = SDLNet_AllocPacket(512);
    UDPpacket *recPacket = SDLNet_AllocPacket(512);
    while(keepAlive == True){
        if(SDLNet_UDP_Recv(serverSocket, recPacket)){
            message recieve;
            memcpy(&recieve, recPacket->data, recPacket->len);
            printf("Client sent: %s", recieve.text);
        }
    }
    printf("Closing server");
    SDLNet_FreePacket(sendPacket);
    SDLNet_FreePacket(recPacket);
*/