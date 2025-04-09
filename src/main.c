#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#include "RenderWindow.h" 
#include "Entity.h"
#include "Vmath.h"

#define WINDOW_W 1280
#define WINDOW_H 720

#define PLAYERSPEED 150
#define JUMPSPEED 400
#define GRAVITYSPEED 98
#define MAX_GRAVITY_SPEED 3.5f

//DynamicArray
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

void addEntity(EntityArray *array, float x, float y, SDL_Texture *texture, int hitboxType) {
    Vec2 *tmp = createVector();
    initVec(tmp, x, y);

    if(array->size == array->capacity) {
        array->capacity *=2;
        array->entities = realloc(array->entities, array->capacity * sizeof(Entity*));
        if(!array->entities){
            printf("Error failed to reallocate memory for array\n");
            return;
        }
    }
    array->entities[array->size] = createEntiy(tmp, texture, hitboxType);
    array->size++;
}

void freeEntityArray(EntityArray *array) {
    for (size_t i = 0; i < array->size; i++) {
        free(array->entities[i]);
    }
    free(array->entities);
    array->entities = NULL;
    array->size = 0;
    array->capacity = 0;
}
//End of DynamicArray

int main(int argv, char** args) {
    if(SDL_Init(SDL_INIT_VIDEO)!=0) {
        printf("Error: %s\n",SDL_GetError());
        return 1;
    }
    //rendera ett fönster  
    RenderWindow *window = create_renderWindow("TTT", WINDOW_W, WINDOW_H);
    //rendera in Texturer för sig
    SDL_Texture *grassTexture = loadTexture("resources/purpg.png", window);
    //laddar Entitys med pekare till texturer
    //Entity *platform0 = createEntiy(100, 50, grassTexture);
    SDL_Texture *player1 = loadTexture("resources/player1.png", window);

    //DynamicArray
    EntityArray platformArray = initEntityArray();
    for(int i = 0; i < (WINDOW_W/(32*4) * 32); i+=32) {
        addEntity(&platformArray, i, 160, grassTexture, HITBOX_32BLOCK);
    }

    /*for(int i = 0; i < (WINDOW_W/(32*4) * 32); i+=32) {
        addEntity(&platformArray, i, WINDOW_H-32, grassTexture, HITBOX_32BLOCK);
    }*/

    addEntity(&platformArray, 0, 32, grassTexture, HITBOX_32BLOCK);
    //addEntity(&platformArray, WINDOW_W-32, 64, grassTexture, HITBOX_32BLOCK);

    Vec2 *playerV = createVector();
    initVec(playerV, 32, 32);
    Entity *player = createEntiy(playerV, player1, HITBOX_PLAYER);

    bool gameRunning = true;

    SDL_Event event;

    const float timestep = 1.0f/60.0f; //Fixed timestep (60 Updates per second)
    Uint32 lastTime = SDL_GetTicks();
    float deltaTime = 0.0f;
    float accumulator = 0.0f;
    float dy = 0.0f;
   
    int jumpTimer = 8;
    bool up, down, left, right, jump;
    up = down = left = right = jump = false;
    bool godMode = false, applyGravity = true, jumpLock = true;
    Vec2 *deltaVelocity = createVector();
    float gravityModifier = 1.0f;

    while(gameRunning) {
        Uint32 currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) / 1000.0f; // ms till sekunder
        lastTime = currentTime;
        accumulator += deltaTime;

        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                gameRunning = false;
            }
            else if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:
                        if (godMode) { up = true; }
                        break;
                    case SDL_SCANCODE_S:
                        if (godMode) { down = true; }
                        break;
                    case SDL_SCANCODE_A:
                        left = true;
                        break;
                    case SDL_SCANCODE_D:
                        right = true;
                        break;
                    case SDL_SCANCODE_T:
                        printf("dT: %f\n", deltaTime);
                        printf("PlayerPos: x=%f, y=%f\n", entityGetX(player), entityGetY(player));
                        printf("PlatformPos: x=%f, y=%f\n", entityGetX(platformArray.entities[0]), entityGetY(platformArray.entities[0]));
                        printf("playerHbox: x=%f, y=%f\n", getHPos(player).x , getHPos(player).y);
                        printf("PlatformHbox: x=%f, y=%f\n", getHPos(platformArray.entities[0]).x, getHPos(platformArray.entities[0]).y);
                        printf("PlatformArray.size: %ld\n", platformArray.size);
                        break;
                    case SDL_SCANCODE_F:
                        updateEntity(player);
                        break;
                    case SDL_SCANCODE_G:
                        if (!godMode) { godMode = true; } else { godMode = false; }
                        break;
                    case SDL_SCANCODE_SPACE:
                        if (!jumpLock) {
                            dy = -1.0 * JUMPSPEED * timestep;
                            gravityModifier = 0.25f;
                            jump = true;
                            jumpLock = true;
                        }
                        break;
                }
            }
            else if(event.type == SDL_KEYUP) {
                switch(event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:
                        up = false;
                        break;
                    case SDL_SCANCODE_S: 
                        down = false;
                        break;
                    case SDL_SCANCODE_A:
                        left = false;
                        break;
                    case SDL_SCANCODE_D: 
                        right = false;
                        break;
                    case SDL_SCANCODE_SPACE:
                        jump = false;
                        jumpTimer = 8;
                        gravityModifier = 1.0f;
                        break;
                }
            }
        }

        //if (jump) { printf("Start jumping!\n"); }
        //else if (!jump && jumpLock) { printf("Stop jumping!\n"); }
        if (accumulator >= timestep) {    
            // Add physics related calculations here...
            if (applyGravity && dy<MAX_GRAVITY_SPEED && !godMode) {
                dy += GRAVITYSPEED * gravityModifier * deltaTime;
            }

            if (jumpTimer>0 && jump) {
                jumpTimer--;
                if (jumpTimer==0) { gravityModifier = 1.0f; }
            }
            
            accumulator = 0;
        }

        if (godMode || (!applyGravity && !jump)) { dy = 0.0f; }

        if (up && !down && !jump) { dy = -1.0 * PLAYERSPEED * deltaTime; }
        if (down && !up) { dy = 1.0 * PLAYERSPEED * deltaTime; }
        initVec(deltaVelocity, 0.0f, dy);
        movePlayer(player, deltaVelocity);
        //blockVertical = checkIntersection(player, platformArray.entities[4], 0, dy);
        // Only check collision if there has been any vertical movement.
        applyGravity = true;
        jumpLock = true;
        for(int i = 0; i < platformArray.size; i++) {
            int toggleBlock = checkIntersection(player, platformArray.entities[i], 0.0f, dy);
            if (toggleBlock==2) {
                applyGravity=false;
                jumpLock=false;
                break;
            }
            else if (toggleBlock) {
                break;
            }
        }

        float dx = 0.0f;
        if (left && !right) { dx = -1.0 * PLAYERSPEED * deltaTime; }
        if (right && !left) { dx = 1.0 * PLAYERSPEED * deltaTime; }
        initVec(deltaVelocity, dx, 0.0f);
        movePlayer(player, deltaVelocity);
        //blockHorizontal = checkIntersection(player, platformArray.entities[4], dx, 0);
        // Only check collision if there has been any horizontal movement.
        if (dx!=0) {
            for(int i = 0; i < platformArray.size; i++) {
                if (checkIntersection(player, platformArray.entities[i], dx, 0.0f)) { break; }
            }
        }
        
        clearWindow(window);
        render(window, player);
        for(int i = 0; i < platformArray.size; i++) {
            render(window, platformArray.entities[i]);
        }
        
        

        display(window);
    }
    
    freeEntityArray(&platformArray); //DynamicArrayFree data
    SDL_DestroyTexture(player1);
    SDL_DestroyTexture(grassTexture);
    //cleanup är egentligen bara destroy window
    cleanUp(window);
    SDL_Quit();

    return 0;
}