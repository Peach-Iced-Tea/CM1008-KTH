#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#include "renderWindow.h" 
#include "entity.h"
#include "vmath.h"
#include "physics.h"
#include "client.h"

// Change GLOBAL_SCALER inside renderWindow.h to change the scaling of the window.
#define WINDOW_W 1280
#define WINDOW_H 720

// DynamicArray     Turn these dynamicArray related functions into its own .c and .h files.
struct entityArray {
    Entity **entities;
    size_t size;
    size_t capacity;
};typedef struct entityArray EntityArray;

EntityArray initEntityArray() {
    EntityArray array;

    array.entities = malloc(4 * sizeof(Entity*));
    if(!array.entities) {
        printf("Error failed allocation of memory for array\n");
        exit(1);
    }
    array.size = 0;
    array.capacity = 4;
    return array;
}

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

void addEntity(EntityArray *array, float x, float y, SDL_Texture *texture, int hitboxType) {
    if(array->size == array->capacity) {
        array->capacity *=2;
        array->entities = realloc(array->entities, array->capacity * sizeof(Entity*));
        if(!array->entities) {
            printf("Error failed to reallocate memory for array\n");
            return;
        }
    }

    array->entities[array->size] = createEntity(createVector(x, y), texture, hitboxType);
    array->size++;
}

void destroyEntityArray(EntityArray *array) {
    for (size_t i = 0; i < array->size; i++) {
        destroyEntity(array->entities[i]);
    }

    free(array->entities);
    array->entities = NULL;
    array->size = 0;
    array->capacity = 0;
}
// End of DynamicArray

int main(int argv, char** args) {
    if(SDL_Init(SDL_INIT_VIDEO)!=0) {
        printf("Error: %s\n",SDL_GetError());
        return 1;
    }

    // Get information about the main display, such as pixel width and pixel height.
    SDL_DisplayMode mainDisplay;
    SDL_GetDesktopDisplayMode(0, &mainDisplay);

    // rendera ett fönster
    RenderWindow *pWindow = createRenderWindow("ToTheTop", WINDOW_W, WINDOW_H);
    // rendera in Texturer för sig
    SDL_Texture *pGrassTexture = loadTexture(pWindow, "resources/purpg.png");
    // laddar Entitys med pekare till texturer
    //Entity *platform0 = createEntiy(100, 50, grassTexture);
    SDL_Texture *pPlayerTexture = loadTexture(pWindow, "resources/player1.png");

    // DynamicArray
    EntityArray platformArray = initEntityArray();
    // Add blocks along the top of the screen.
    //for(int i = 0; i < (WINDOW_W/(32*GLOBAL_SCALER) * 32); i+=32) {
    //    addEntity(&platformArray, i, 0.0f, grassTexture, HITBOX_FULL_BLOCK);
    //}

    // Add blocks along the middle of the y-axis.
    /*for(int i = 0; i < (WINDOW_W/(32*GLOBAL_SCALER) * 32); i+=32) {
        addEntity(&platformArray, i, (WINDOW_H*0.5f-32*GLOBAL_SCALER*0.5f)/GLOBAL_SCALER, grassTexture, HITBOX_FULL_BLOCK);
    }*/

    // Add blocks along the bottom of the screen.
    for(int i = 0; i < (WINDOW_W/(32*GLOBAL_SCALER) * 32); i+=32) {
        addEntity(&platformArray, i, (WINDOW_H-32*GLOBAL_SCALER)/GLOBAL_SCALER, pGrassTexture, HITBOX_FULL_BLOCK);
    }

    for (int i = 0; i < 6*32; i+=32) {
        addEntity(&platformArray, i, 96, pGrassTexture, HITBOX_FULL_BLOCK);
        addEntity(&platformArray, i, 128, pGrassTexture, HITBOX_FULL_BLOCK);
        addEntity(&platformArray, i, 160, pGrassTexture, HITBOX_FULL_BLOCK);
        addEntity(&platformArray, i+6*32, 192, pGrassTexture, HITBOX_FULL_BLOCK);
    }

    for (int i = 0; i < 4*32; i+=32) {
        addEntity(&platformArray, i+7*32, 96, pGrassTexture, HITBOX_FULL_BLOCK);
    }

    addEntity(&platformArray, 0, 32, pGrassTexture, HITBOX_HALF_BLOCK);
    addEntity(&platformArray, (WINDOW_W-32*GLOBAL_SCALER)/GLOBAL_SCALER, 32, pGrassTexture, HITBOX_FULL_BLOCK);

    Entity *pPlayer = createEntity(createVector(32, 32), pPlayerTexture, HITBOX_PLAYER);

    bool gameRunning = true;

    SDL_Event event;

    const float timestep = 1.0f/60.0f; // Fixed timestep (60 Updates per second)
    Uint32 lastTime = SDL_GetTicks();
    float deltaTime = 0.0f;
    float accumulator = 0.0f;
   
    int jumpTimer = 0;
    bool up, down, left, right, jump;
    up = down = left = right = jump = false;
    bool godMode = false, applyVelocity = false;
    float gravityModifier = 1.0f;   // Changes how much of GRAVITY_ACCELERATION that is applied to a player.
    Vec2 currentDirection = createVector(0.0f, 0.0f); // Contains the current direction the player should move.
    
    // Added in networking Branch
    returnData playerData;
    client playerCli;
    int gameState=0,connectionState=0;
    bool establishConnection=true;
    char ipAddr[16];
    int ms = 0;
    playerData.playerNumber = 0;

    while(gameRunning) {
        switch(gameState){
            case GAME_CONNECTING:
                while(establishConnection){
                    switch(connectionState){
                        case CONNECTION_SETUP:
                            gameMenu(pWindow,&event,&gameState, ipAddr);
                            //host/join
                            connectionState=CONNECTION_SERVER;
                            break;
                        case CONNECTION_SERVER:
                            playerCli.data.ID = -1;

                            //koppla upp
                            serverConnect(ipAddr, 50505, &playerCli);
                            connectionState=CONNECTION_WAIT;
                            break;
                        case CONNECTION_WAIT:
                            //vänta på andra spelare
                            establishConnection=false;
                            connectionState=CONNECTION_SETUP;
                            break;
                    }
                }
                gameState=GAME_RUNNING;
                break;
            case GAME_RUNNING: 


                Uint32 currentTime = SDL_GetTicks();
                deltaTime = (currentTime - lastTime) * 0.001f; // ms till sekunder
                lastTime = currentTime;
                accumulator += deltaTime;

                currentDirection.x = 0.0f;
                currentDirection.y = 0.0f;
                while(SDL_PollEvent(&event)) {  // Turn this into a function, maybe add a struct containing a Vec2* and some other variables.
                    if(event.type == SDL_QUIT) {
                        gameRunning = false;
                    }
                    else if(event.type == SDL_KEYDOWN) {
                        switch(event.key.keysym.scancode) {
                            case SDL_SCANCODE_ESCAPE:
                                gameRunning = false;
                                break;
                            case SDL_SCANCODE_W:
                                if (godMode && !up) {
                                    up = true;
                                    applyVelocity = true;
                                }
                                break;
                            case SDL_SCANCODE_S:
                                if (godMode && !down) {
                                    down = true;
                                    applyVelocity = true;
                                }
                                break;
                            case SDL_SCANCODE_A:
                                if (!left) {
                                    left = true;
                                    applyVelocity = true;
                                }
                                break;
                            case SDL_SCANCODE_D:
                                if (!right) {
                                    right = true;
                                    applyVelocity = true;
                                }
                                break;
                            case SDL_SCANCODE_T:
                                printf("dT: %f\n", deltaTime);
                                printf("PlayerPos: x=%f, y=%f\n", getPosition(pPlayer).x, getPosition(pPlayer).y);
                                printf("PlatformPos: x=%f, y=%f\n", getPosition(platformArray.entities[0]).x, getPosition(platformArray.entities[0]).y);
                                printf("playerHbox: x=%f, y=%f\n", getHitboxPosition(getHitbox(pPlayer)).x , getHitboxPosition(getHitbox(pPlayer)).y);
                                printf("PlatformHbox: x=%f, y=%f\n", getHitboxPosition(getHitbox(platformArray.entities[0])).x, getHitboxPosition(getHitbox(platformArray.entities[0])).y);
                                printf("PlatformArray.size: %ld\n", platformArray.size);
                                printf("playerVelocity.x: %f\n", getVelocity(pPlayer).x);
                                printf("playerVelocity.y: %f\n", getVelocity(pPlayer).y);
                                break;
                            case SDL_SCANCODE_G:
                                if (!godMode) {
                                    applyVelocity = true;
                                    gravityModifier = 0.0f;
                                    godMode = true;
                                }
                                else {
                                    applyVelocity = true;
                                    godMode = false;
                                }
                                break;
                            case SDL_SCANCODE_SPACE:
                                if (!godMode) { jump = true; }
                                break;
                        }
                    }
                    else if(event.type == SDL_KEYUP) {
                        switch(event.key.keysym.scancode) {
                            case SDL_SCANCODE_W:
                                up = false;
                                if (godMode) {
                                    applyVelocity = true;
                                }
                                break;
                            case SDL_SCANCODE_S:
                                down = false;
                                if (godMode) {
                                    applyVelocity = true;
                                }
                                break;
                            case SDL_SCANCODE_A:
                                left = false;
                                applyVelocity = true;
                                break;
                            case SDL_SCANCODE_D:
                                right = false;
                                applyVelocity = true;
                                break;
                            case SDL_SCANCODE_SPACE:
                                jump = false;
                                break;
                        }
                    }
                }

                if (up && godMode) { currentDirection.y = -1.0f; }
                if (down && godMode) { currentDirection.y += 1.0f; }
                if (left) { currentDirection.x = -1.0f; }
                if (right) { currentDirection.x += 1.0f; }

                if (jump && gravityModifier == 0.0f) {
                    jumpTimer = 10;
                    gravityModifier = 0.50f;
                    setVelocityY(pPlayer, -JUMP_VELOCITY);
                }
                else if (!jump && jumpTimer > 0) {
                    jumpTimer = 0;
                    gravityModifier = 1.0f;
                    if (getVelocity(pPlayer).y<=0.0f) { setVelocityY(pPlayer, 0.0f); }
                }

                if (applyVelocity) {
                    if (godMode) {
                        setVelocityY(pPlayer, currentDirection.y*MAX_PLAYER_VELOCITY*0.75f);
                    }

                    setVelocityX(pPlayer, currentDirection.x*(PLAYER_VELOCITY*!godMode + MAX_PLAYER_VELOCITY*0.75f*godMode));
                    setAccelerationX(pPlayer, currentDirection.x*PLAYER_ACCELERATION*!godMode);

                    applyVelocity = false;
                }

                if (accumulator >= timestep) {    
                    // Add physics related calculations here..
                    ms++;
                    if (jumpTimer > 0) {
                        jumpTimer--;
                        if (jumpTimer == 0) { gravityModifier = 1.0f; }
                    }
                    
                    if(ms >= 60){
                        recPacket(&playerCli, &playerData);
                        sendPacket(&playerCli);
                        ms = 0;
                    }

                    if (!godMode) {
                        if (gravityModifier == 0.0f) { setVelocityY(pPlayer, 0.0f); }
                        float gravity = GRAVITY_ACCELERATION*gravityModifier;
                        if (getAcceleration(pPlayer).y != gravity) {
                            setAccelerationY(pPlayer, gravity);
                        }
                    }

                    updateVelocity(pPlayer, timestep);
                    accumulator -= timestep;
                }

                updatePosition(pPlayer, deltaTime);
                if (!godMode && jumpTimer == 0) { gravityModifier = 1.0f; }

                Hitbox *pPlayerHitbox = getHitbox(pPlayer);
                for(int i = 0; i < platformArray.size; i++) {
                    Hitbox *pEntityHitbox = getHitbox(platformArray.entities[i]);
                    if (checkCollision(pPlayerHitbox, pEntityHitbox)) {
                        Vec2 correction = rectVsRect(pPlayerHitbox, pEntityHitbox);
                        collisionResponse(pPlayer, correction);
                        switch (hitboxOrientation(pPlayerHitbox, pEntityHitbox)) {
                            case OBJECT_IS_NORTH:
                                gravityModifier = 0.0f;
                                break;
                            case OBJECT_IS_SOUTH:
                                break;
                            case OBJECT_IS_WEST:
                                break;
                            case OBJECT_IS_EAST:
                                break;
                        }
                    }
                }

                clearWindow(pWindow);
                renderEntity(pWindow, pPlayer);
                for(int i = 0; i < platformArray.size; i++) {
                    renderEntity(pWindow, platformArray.entities[i]);
                }
                
                displayWindow(pWindow);
            break;
        case GAME_CLOSING:
            break;
        }
    }
    
    destroyEntity(pPlayer);
    destroyEntityArray(&platformArray);
    SDL_DestroyTexture(pPlayerTexture);
    SDL_DestroyTexture(pGrassTexture);
    destroyRenderWindow(pWindow);
    SDL_Quit();

    return 0;
}