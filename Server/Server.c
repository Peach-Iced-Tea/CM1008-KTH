#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

typedef struct vec2{
    float x, y;
} Vec2;

typedef struct DataSection{
    int ID;
    Vec2 position;
} dataSection;

typedef struct ReturnData{
    int playerNumber;
    dataSection players[4];
} returnData;

int SDL_main(int argc, char* argv[])
{
    SDLNet_Init();
    UDPsocket serverSocket = SDLNet_UDP_Open(50505);
    UDPpacket* sendPacket = SDLNet_AllocPacket(512);
    UDPpacket* recPacket = SDLNet_AllocPacket(512);
    int clients = 0;
    IPaddress clientIPs[10];
    returnData playerData;
    playerData.playerNumber = 0;

    while (clients < 4){
        if(SDLNet_UDP_Recv(serverSocket, recPacket)){
            dataSection recieved;
            memcpy(&recieved, recPacket->data, recPacket->len);

            if (recieved.ID == -1){
                playerData.playerNumber++;
                // printf("Debug: New client, total clients %d\n", playerData.playerNumber);
                clientIPs[clients] = recPacket->address;
                
                playerData.players[clients].ID = clients;
                playerData.players[clients].position = recieved.position;

                int returnPacket = clients++;

                sendPacket->address = recPacket->address;
                sendPacket->len = sizeof(int);
                memcpy(sendPacket->data, &returnPacket, sendPacket->len);

                SDLNet_UDP_Send(serverSocket, -1, sendPacket);

            } else {
                playerData.players[recieved.ID].position = recieved.position;
                // printf("Debug: player pos %f %f\n", recieved.position.x, recieved.position.y);

                sendPacket->len = sizeof(returnData);
                memcpy(sendPacket->data, &playerData, sendPacket->len);
                sendPacket->address = recPacket->address;
                SDLNet_UDP_Send(serverSocket, -1, sendPacket);
                // printf("%d players connected.\n", playerData.playerNumber);
                // for(int i = 0; i < playerData.playerNumber; i++){
                //     sendPacket->address = clientIPs[i];
                //     SDLNet_UDP_Send(serverSocket, -1, sendPacket);
                //     // printf("Data from %d sent to player %d\n", recieved.ID, i);
                // }
                
            }

        };
    }

    printf("Closing server");

    return 0;
}