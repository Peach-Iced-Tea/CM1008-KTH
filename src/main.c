#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#include "RenderWindow.h"
#include "Entity.h"
#include "Vmath.h"

#define WINDOW_W 1920
#define WINDOW_H 1080

#define PLAYERSPEED 150

//DynamicArray
struct entityArray {
    Entity **entities;
    size_t size;
    size_t capacity;
};typedef struct entityArray EntityArray;

EntityArray initEntityArray() {
    EntityArray array;

    array.entities = malloc(3 * sizeof(Entity*));
    if(!array.entities) {
        printf("Error failed allocation of memory for array\n");
        exit(1);
    }
    array.size = 0;
    array.capacity = 3;
    return array;
}

void addEntity(EntityArray *array, float x, float y, SDL_Texture *texture, int hitboxType) {
    Vector2f v; 
    initVec(&v, x, y);

    if(array->size == array->capacity) {
        array->capacity *=2;
        array->entities = realloc(array->entities, array->capacity * sizeof(Entity*));
        if(!array->entities){
            printf("Error failed to reallocate memory for array\n");
            return;
        }
    }
    array->entities[array->size] = createEntiy(v, texture, hitboxType);
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
    RenderWindow *window = create_renderWindow("TTT", 1920, 1080);
    //rendera in Texturer för sig
    SDL_Texture *grassTexture = loadTexture("resources/purpg.png", window);
    //laddar Entitys med pekare till texturer
    //Entity *platform0 = createEntiy(100, 50, grassTexture);
    SDL_Texture *player1 = loadTexture("resources/player1.png", window);


    //DynamicArray
    EntityArray platformArray = initEntityArray();
    for(int i = 0; i < ((WINDOW_W/(32*4)) * 32); i+=32) {
        addEntity(&platformArray, i, 94, grassTexture, HITBOX_32BLOCK);
    }
    addEntity(&platformArray, 0, 62, grassTexture, HITBOX_32BLOCK);

    addEntity(&platformArray, 128, 62, grassTexture, HITBOX_32BLOCK);

    Vector2f playerV;
    initVec(&playerV, 32, 32);
    Entity *player = createEntiy(playerV, player1, HITBOX_PLAYER);



    bool gameRunning = true;

    SDL_Event event;

    const float timestep = 1.0f/60.0f; //Fixed timestep (60 Updates per second)
    Uint32 lastTime = SDL_GetTicks();
    float deltaTime = 0.0f;
    float accumulator = 0.0f;
   


    int blockVertical = 0;  // BlockVertical: 0 = no block, 1 = block up movement, 2 = block down movement
    int blockHorizontal = 0; // BlockHorizontal: 0 = no block, 1 = block left movement, 2 = block right movement
    bool up, down, left, right;
    up = down = left = right = false;



    while(gameRunning) {
        Uint32 currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) / 1000.0f; // ms till sekunder
        lastTime = currentTime;
        accumulator += deltaTime;
        
        while(accumulator >= timestep) {    
            // Add physics related calculations here...
            accumulator -= timestep;
        }

        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                gameRunning = false;
            }
            else if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:
                        // Only set up to 'true' if up movement isn't blocked.
                        if (blockVertical!=BLOCK_UP || 1) {
                            up = true;
                            blockVertical = blockHorizontal = 0;
                        } else {
                            up = false;
                        }
                        break;
                    case SDL_SCANCODE_S:
                        // Only set down to 'true' if down movement isn't blocked.
                        if (blockVertical!=BLOCK_DOWN || 1) {
                            down = true;
                            blockVertical = blockHorizontal = 0;
                        } else {
                            down = false;
                        }
                        break;
                    case SDL_SCANCODE_A: 
                        // Only set left to 'true' if left movement isn't blocked.
                        if (blockHorizontal!=BLOCK_LEFT || 1) {
                            left = true;
                            blockVertical = blockHorizontal = 0;
                        } else {
                            left = false;
                        }
                        break;
                    case SDL_SCANCODE_D:
                        // Only set right to 'true' if right movement isn't blocked.
                        if (blockHorizontal!=BLOCK_RIGHT || 1) {
                            right = true;
                            blockVertical = blockHorizontal = 0;
                        } else {
                            right = false;
                        }
                        break;
                    case SDL_SCANCODE_T:
                        printf("dT: %f\n", deltaTime);
                        printf("PlayerRect: x=%f, y=%f\n", getPos(player).x, getPos(player).y );
                        printf("PlatformRect: x=%f, y=%f\n", getPos(platformArray.entities[0]).x, getPos(platformArray.entities[0]).y);
                        printf("playerHbox: x=%f, y=%f\n", getHPos(player).x , getHPos(player).y);
                        printf("PlatformHbox: x=%f, y=%f\n", getHPos(platformArray.entities[0]).x, getHPos(platformArray.entities[0]).y);
                        printf("Blocks: %d\n", platformArray.size);
                        break;
                    case SDL_SCANCODE_F:
                        updateEntity(player);
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
                }
            }
        }

        float dx = 0.0f, dy = 0.0f;
        if (up && !down) { dy = -1.0 * PLAYERSPEED * deltaTime; }
        if (down && !up) { dy = 1.0 * PLAYERSPEED * deltaTime; }
        movePlayer(player, 0.0f, dy);
        //blockVertical = checkIntersection(player, platformArray.entities[4], 0, dy);
        // Only check collision if there has been any vertical movement.
        if (dy<0 || dy>0) {
            for(int i = 0; i < platformArray.size; i++) {
                blockVertical = checkIntersection(player, platformArray.entities[i], 0, dy);
                if (blockVertical) { break; }
            }
        }

        if (left && !right) { dx = -1.0 * PLAYERSPEED * deltaTime; }
        if (right && !left) { dx = 1.0 * PLAYERSPEED * deltaTime; }
        movePlayer(player, dx, 0.0f);
        //blockHorizontal = checkIntersection(player, platformArray.entities[4], dx, 0);
        // Only check collision if there has been any horizontal movement.
        if (dx<0 || dx>0) {
            for(int i = 0; i < platformArray.size; i++) {
                blockHorizontal = checkIntersection(player, platformArray.entities[i], dx, 0);
                if (blockHorizontal) { break; }
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