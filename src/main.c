#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>


#include "RenderWindow.h" 
#include "Entity.h"


#define WINDOW_W 1280
#define WINDOW_H 720


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

    //ladda flera entity till array för snabbare distributering
    Entity *platforms[3] = {createEntiy(0, 0, grassTexture),
                            createEntiy(32, 0, grassTexture),
                            createEntiy(32, 32, grassTexture)};
    

    //Testar andra entitet Texturer!
    //SDL_Texture *playerTexture = loadTexture("resources/player1.png", window);
    //Entity *player = createEntiy(100, 60, playerTexture);


    bool gameRunning = true;

    SDL_Event event;

    while(gameRunning){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                gameRunning = false;
            }
        }
        clearWindow(window);

        for(int i = 0; i < 3; i++){
            render(window, platforms[i]);
        }
        //render(window, platform0);

        //render(window, player); //Testar andra!
        display(window);
    }

    SDL_DestroyTexture(grassTexture);
    //cleanup är egentligen bara destroy window
    cleanUp(window);
    SDL_Quit();

    return 0;
}