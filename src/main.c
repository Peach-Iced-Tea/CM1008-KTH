#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#include "renderWindow.h" 
#include "entity.h"
#include "vmath.h"
#include "physics.h"

// Change GLOBAL_SCALER inside renderWindow.h to change the scaling of the window.
#define WINDOW_W 1920
#define WINDOW_H 1080

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

void freeEntityArray(EntityArray *array) {
    for (size_t i = 0; i < array->size; i++) {
        free(array->entities[i]);
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
    RenderWindow *window = createRenderWindow("ToTheTop", WINDOW_W, WINDOW_H);
    // rendera in Texturer för sig
    SDL_Texture *grassTexture = loadTexture("resources/purpg.png", window);
    // laddar Entitys med pekare till texturer
    //Entity *platform0 = createEntiy(100, 50, grassTexture);
    SDL_Texture *player1 = loadTexture("resources/player1.png", window);

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
        addEntity(&platformArray, i, (WINDOW_H-32*GLOBAL_SCALER)/GLOBAL_SCALER, grassTexture, HITBOX_FULL_BLOCK);
    }

    for (int i = 0; i < 6*32; i+=32) {
        addEntity(&platformArray, i, 96, grassTexture, HITBOX_FULL_BLOCK);
        addEntity(&platformArray, i, 128, grassTexture, HITBOX_FULL_BLOCK);
        addEntity(&platformArray, i, 160, grassTexture, HITBOX_FULL_BLOCK);
        addEntity(&platformArray, i+6*32, 192, grassTexture, HITBOX_FULL_BLOCK);
    }

    for (int i = 0; i < 4*32; i+=32) {
        addEntity(&platformArray, i+7*32, 96, grassTexture, HITBOX_FULL_BLOCK);
    }

    addEntity(&platformArray, 0, 32, grassTexture, HITBOX_HALF_BLOCK);
    addEntity(&platformArray, (WINDOW_W-32*GLOBAL_SCALER)/GLOBAL_SCALER, 32, grassTexture, HITBOX_FULL_BLOCK);

    Entity *player = createEntity(createVector(32, 32), player1, HITBOX_PLAYER);

    bool gameRunning = true;

    SDL_Event event;

    const float timestep = 1.0f/60.0f; // Fixed timestep (60 Updates per second)
    Uint32 lastTime = SDL_GetTicks();
    float deltaTime = 0.0f;
    float accumulator = 0.0f;
   
    int jumpTimer = 0;
    bool up, down, left, right, jump;
    up = down = left = right = jump = false;
    bool godMode = false, jumpLock = true, applyVelocity = false;
    float gravityModifier = 1.0f;   // Changes how much of GRAVITY_ACCELERATION that is applied to a player.
    Vec2 currentDirection = createVector(0.0f, 0.0f); // Contains the current direction the player should move.

    while(gameRunning) {
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
                        printf("PlayerPos: x=%f, y=%f\n", getPosition(player).x, getPosition(player).y);
                        printf("PlatformPos: x=%f, y=%f\n", getPosition(platformArray.entities[0]).x, getPosition(platformArray.entities[0]).y);
                        printf("playerHbox: x=%f, y=%f\n", getHitboxPosition(getHitbox(player)).x , getHitboxPosition(getHitbox(player)).y);
                        printf("PlatformHbox: x=%f, y=%f\n", getHitboxPosition(getHitbox(platformArray.entities[0])).x, getHitboxPosition(getHitbox(platformArray.entities[0])).y);
                        printf("PlatformArray.size: %ld\n", platformArray.size);
                        printf("playerVelocity.x: %f\n", getVelocity(player).x);
                        printf("playerVelocity.y: %f\n", getVelocity(player).y);
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

        if (jump && !jumpLock) {
            jumpTimer = 20;
            gravityModifier = 0.50f;
            setVelocityY(player, -JUMP_VELOCITY);
        }
        else if (!jump && jumpTimer > 0) {
            jumpTimer = 0;
            gravityModifier = 1.0f;
            if (getVelocity(player).y<=0.0f) { setVelocityY(player, PLAYER_VELOCITY*0.25f); }
        }

        if (applyVelocity) {
            if (godMode) {
                setVelocityY(player, currentDirection.y*MAX_PLAYER_VELOCITY*0.75f);
            }

            setVelocityX(player, currentDirection.x*(PLAYER_VELOCITY*!godMode + MAX_PLAYER_VELOCITY*0.75f*godMode));
            setAccelerationX(player, currentDirection.x*PLAYER_ACCELERATION*!godMode);

            applyVelocity = false;
        }

        if (accumulator >= timestep) {    
            // Add physics related calculations here...
            if (jumpTimer > 0) {
                jumpTimer--;
                if (jumpTimer == 0) { gravityModifier = 1.0f; }
            }

            if (!godMode) {
                if (gravityModifier == 0.0f) { setVelocityY(player, 0.0f); }
                float gravity = GRAVITY_ACCELERATION*gravityModifier;
                if (getAcceleration(player).y != gravity) {
                    setAccelerationY(player, gravity);
                }
            }

            updateVelocity(player, timestep);
            accumulator -= timestep;
        }

        updatePosition(player, deltaTime);
        gravityModifier = 1.0f;
        jumpLock = true;

        Hitbox *playerHitbox = getHitbox(player);
        for(int i = 0; i < platformArray.size; i++) {
            Hitbox *entityHitbox = getHitbox(platformArray.entities[i]);
            if (checkCollision(playerHitbox, entityHitbox)) {
                Vec2 correction = rectVsRect(playerHitbox, entityHitbox);
                collisionResponse(player, correction);
                if (correction.y>0.0f) { continue; }

                if (correction.x!=0.0f) { continue; }

                if (hitboxIsAbove(playerHitbox, entityHitbox)) {
                    gravityModifier = 0.0f;
                    jumpLock = false;
                }
            }
        }

        clearWindow(window);
        renderEntity(window, player);
        for(int i = 0; i < platformArray.size; i++) {
            renderEntity(window, platformArray.entities[i]);
        }
        
        displayWindow(window);
    }
    
    free(player);
    freeEntityArray(&platformArray); //DynamicArrayFree data
    SDL_DestroyTexture(player1);
    SDL_DestroyTexture(grassTexture);
    //cleanup är egentligen bara destroy window
    cleanUp(window);
    SDL_Quit();

    return 0;
}