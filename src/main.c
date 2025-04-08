#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>


#include "RenderWindow.h" 
#include "Entity.h"
#include "Vmath.h"



#define WINDOW_W 1280
#define WINDOW_H 720

//DynamicArray
struct entityArray{
    Entity **entities;
    size_t size;
    size_t capacity;
};typedef struct entityArray EntityArray;

EntityArray initEntityArray(){
    EntityArray array;

    array.entities = malloc(3 * sizeof(Entity*));
    if(!array.entities){
        printf("Error failed allocation of memory for array\n");
        exit(1);
    }
    array.size = 0;
    array.capacity = 3;
    return array;
}

void addEntity(EntityArray *array, float x, float y, SDL_Texture *texture)
{
    Vector2f v; 
    initVec(&v, x, y);

    if(array->size == array->capacity){
        array->capacity *=2;
        array->entities = realloc(array->entities, array->capacity * sizeof(Entity*));
        if(!array->entities){
            printf("Error failed to reallocate memory for array\n");
            return;
        }
    }

    array->entities[array->size] = createEntiy(v, texture);
    array->size++;
}

void freeEntityArray(EntityArray *array)
{
    for (size_t i = 0; i < array->size; i++) {
        free(array->entities[i]);
    }
    free(array->entities);
    array->entities = NULL;
    array->size = 0;
    array->capacity = 0;
}
//End of DynamicArray


int main(int argv, char** args)
{    
    if(SDL_Init(SDL_INIT_VIDEO)!=0){
        printf("Error: %s\n",SDL_GetError());
        return 1;
    }
    //rendera ett fönster  
    RenderWindow *window = create_renderWindow("Scratch", 1280, 720);
    //rendera in Texturer för sig
    SDL_Texture *grassTexture = loadTexture("resources/purpg.png", window);
    //laddar Entitys med pekare till texturer
    //Entity *platform0 = createEntiy(100, 50, grassTexture);
    SDL_Texture *player1 = loadTexture("resources/player1.png", window);


    //DynamicArray
    EntityArray platformArray = initEntityArray();
    for(int i = 0; i < ((WINDOW_W/32*4) * 32); i+=32)
    {
        addEntity(&platformArray, i, 94, grassTexture);
    }

    addEntity(&platformArray, 94, 32, grassTexture);

    Vector2f playerV;
    initVec(&playerV, 32, 32);
    
    Entity *player = createEntiy(playerV, player1);



    bool gameRunning = true;

    SDL_Event event;


    const float timestep = 1.0f/60.0f; //Fixed timestep (60 Updates per second)
    Uint32 lastTime = SDL_GetTicks();
    float deltaTime = 0.0f;
    float accumulator = 0.0f;
    bool up, down, left, right;
    up = down = left = right = false;
    int timer = 0;




    while(gameRunning){

        Uint32 currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) / 1000.0f; // ms till sekunder
        lastTime = currentTime;
        accumulator += deltaTime;
        timer++;

        while(accumulator >= timestep){
            // Lägg till physics här
            accumulator -= timestep;
        }

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                gameRunning = false;
            }
            else if(event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.scancode)
                {
                    case SDL_SCANCODE_W: 
                        up = true;
                        break;
                    case SDL_SCANCODE_S:
                        down = true;
                        break;
                    case SDL_SCANCODE_A: 
                        left = true;
                        break;
                    case SDL_SCANCODE_D: 
                        right = true;
                        break;
                    case SDL_SCANCODE_T:
                        printf("dT: %f\n", deltaTime);
                        printf("timer: %d\n", timer);
                        break;
                }
            }
            else if(event.type == SDL_KEYUP)
            {
                switch(event.key.keysym.scancode)
                {
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
        if (up && !down) { dy = -1.0 * 100 * deltaTime; }
        if (down && !up) { dy = 1.0 * 100 * deltaTime; }
        if (left && !right) { dx = -1.0 * 100 * deltaTime; }
        if (right && !left) { dx = 1.0 * 100 * deltaTime; }
        movePlayer(player, dx, dy);
        //updateEntity(player);
        

        clearWindow(window);
        
        render(window, player);

        for(int i = 0; i < platformArray.size; i++)
        {
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