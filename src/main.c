#include "main.h"

// Change GLOBAL_SCALER inside renderWindow.h to change the scaling of the window.
#define WINDOW_W 1920
#define WINDOW_H 1080

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
    DynamicArray *pPlatformArray = createDynamicArray(ARRAY_ENTITIES);
   
    // Add blocks along the bottom of the screen.
    for(int i = 0; i < (WINDOW_W/(32*GLOBAL_SCALER) * 32); i+=32) {
        addEntity(pPlatformArray, i, (WINDOW_H-32*GLOBAL_SCALER)/GLOBAL_SCALER, pGrassTexture, HITBOX_FULL_BLOCK);
    }

    for (int i = 0; i < 6*32; i+=32) {
        addEntity(pPlatformArray, i, 96, pGrassTexture, HITBOX_FULL_BLOCK);
        addEntity(pPlatformArray, i, 128, pGrassTexture, HITBOX_FULL_BLOCK);
        addEntity(pPlatformArray, i, 160, pGrassTexture, HITBOX_FULL_BLOCK);
        addEntity(pPlatformArray, i+6*32, 192, pGrassTexture, HITBOX_FULL_BLOCK);
    }

    for (int i = 0; i < 4*32; i+=32) {
        addEntity(pPlatformArray, i+7*32, 96, pGrassTexture, HITBOX_FULL_BLOCK);
    }

    addEntity(pPlatformArray, 0, 32, pGrassTexture, HITBOX_HALF_BLOCK);
    addEntity(pPlatformArray, (WINDOW_W-32*GLOBAL_SCALER)/GLOBAL_SCALER, 32, pGrassTexture, HITBOX_FULL_BLOCK);

    //Entity *pPlayer = createEntity(createVector(32, 32), pPlayerTexture, HITBOX_PLAYER);

    Player *pPlayer = createPlayer(createVector(32.0f, 32.0f), pPlayerTexture);

//--------------------------------------------------------------------------------------------------------------------//

    //RotateObject
    Entity *pMObject = createEntity(createVector(256, 256), pPlayerTexture, HITBOX_FULL_BLOCK);

    int x, y;
    Vec2 mouseVector = createVector(0.0f, 0.0f);
   
//--------------------------------------------------------------------------------------------------------------------//

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

    while(gameRunning) {
        Uint32 currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) * 0.001f; // ms till sekunder
        lastTime = currentTime;
        accumulator += deltaTime;

        SDL_GetMouseState(&x, &y);
        mouseVector.x = (float)x/GLOBAL_SCALER;
        mouseVector.y = (float)y/GLOBAL_SCALER;
          
        gameRunning = playerHandleInput(pPlayer);

        if (playerGetMouseClick(pPlayer)) {
            if (vectorLength(mouseVector, getMidPoint(playerGetBody(pPlayer))) < 240.0f) {
                if (touching(getHitbox(pMObject), mouseVector)) {
                    playerSetState(pPlayer, ROTATING); //LOGIK I PLAYER.C 
                    float radius = vectorLength(getMidPoint(playerGetBody(pPlayer)), getMidPoint(pMObject));
                    float alpha = vectorGetAngle(getMidPoint(playerGetBody(pPlayer)), getMidPoint(pMObject));
                    playerSetRadius(pPlayer, radius);
                    playerSetReferenceAngle(pPlayer, alpha);
                }
            }
        }

        if (accumulator >= timestep) {    
            // Add physics related calculations here...
            playerUpdateState(pPlayer, timestep);
            accumulator -= timestep;
        }

        switch (playerGetState(pPlayer)) {
            case ROTATING:
                Vec2 rotateVelocity;
                Vec2 newRotPos = playerUpdatePosition(pPlayer, deltaTime);
                vectorSub(&rotateVelocity, newRotPos, getMidPoint(pMObject));
                setVelocityX(pMObject, rotateVelocity.x);
                setVelocityY(pMObject, rotateVelocity.y);
                updatePosition(pMObject, 1.0f);
                break;
            default:
                playerUpdatePosition(pPlayer, deltaTime);
                break;
        }
            
        bool standingOnPlatform = false;
        for (int i = 0; i < arrayGetSize(pPlatformArray); i++) {
            if (playerCheckCollision(pPlayer, arrayGetObject(pPlatformArray, i)) == OBJECT_IS_NORTH) {
                standingOnPlatform = true;
            }
        }

        if (!standingOnPlatform) { playerSetState(pPlayer, FALLING); }

        clearWindow(pWindow);
        if(playerGetMouseClick(pPlayer)) {
            drawLine(pWindow, mouseVector, getMidPoint(playerGetBody(pPlayer)));
        } 
        renderEntity(pWindow, playerGetBody(pPlayer));
        renderEntity(pWindow, pMObject);
        for(int i = 0; i < arrayGetSize(pPlatformArray); i++) {
            renderEntity(pWindow, arrayGetObject(pPlatformArray, i));
        }
        
        displayWindow(pWindow);
    }
    
    destroyPlayer(pPlayer);
    destroyDynamicArray(pPlatformArray);
    SDL_DestroyTexture(pPlayerTexture);
    SDL_DestroyTexture(pGrassTexture);
    destroyRenderWindow(pWindow);
    SDL_Quit();

    return 0;
}