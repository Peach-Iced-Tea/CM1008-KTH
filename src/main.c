#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>


#include "RenderWindow.h" 
#include "Entity.h"


#define WINDOW_W 1280
#define WINDOW_H 720

//testing DynamicArrays
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
    if(array->size == array->capacity){
        array->capacity *=2;
        array->entities = realloc(array->entities, array->capacity * sizeof(Entity*));
        if(!array->entities){
            printf("Error failed to reallocate memory for array\n");
            return;
        }
    }

    array->entities[array->size] = createEntiy(x, y, texture);
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
//End of DynamicArrays

int main(int argv, char** args)
{    
    if(SDL_Init(SDL_INIT_VIDEO)!=0){
        printf("Error: %s\n",SDL_GetError());
        return 1;
    }
    //rendera ett fönster (The fönster)
    RenderWindow *window = create_renderWindow("Scratch", 1280, 720);
    //rendera in Texturer för sig
    SDL_Texture *grassTexture = loadTexture("resources/purpg.png", window);
    //laddar Entitys med pekare till texturer
    //Entity *platform0 = createEntiy(100, 50, grassTexture);

    //DynamicArray
    EntityArray platformArray = initEntityArray();
    for(int i = 0; i < ((WINDOW_W/32*4) * 32); i+=32)
    {
        addEntity(&platformArray, i, 94, grassTexture);
    }


    bool gameRunning = true;

    SDL_Event event;

    while(gameRunning){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                gameRunning = false;
            }
        }
        clearWindow(window);
        
        for(int i = 0; i < platformArray.size; i++)
        {
            render(window, platformArray.entities[i]);
        }


        display(window);
    }

    freeEntityArray(&platformArray); //DynamicArrayFree data

    SDL_DestroyTexture(grassTexture);
    //cleanup är egentligen bara destroy window
    cleanUp(window);
    SDL_Quit();

    return 0;
}