#include "main.h"

int main(int argv, char** args) {
    if(SDL_Init(SDL_INIT_VIDEO)!=0) {
        printf("Error: %s\n",SDL_GetError());
        return 1;
    }

    // Get information about the main display, such as pixel width and pixel height.
    SDL_DisplayMode mainDisplay;
    SDL_GetDesktopDisplayMode(0, &mainDisplay);

    // rendera ett fönster
    RenderWindow *pWindow = createRenderWindow("ToTheTop", mainDisplay.w, mainDisplay.h);
    // rendera in Texturer för sig
    SDL_Texture *pGrassTexture = loadTexture(pWindow, "resources/purpg.png");
    // laddar Entitys med pekare till texturer
    SDL_Texture *pPlayerTexture = loadTexture(pWindow, "resources/spriteSheetPlayer1.png");

    Player *pPlayer = createPlayer(createVector(32.0f, 32.0f), pPlayerTexture);
    //Player *pReference = createPlayer(createVector(128.0f, 32.0f), pPlayerTexture);
    Entity *pMObject = createEntity(createVector(256, 256), pPlayerTexture, 0, HITBOX_FULL_BLOCK);

    // DynamicArray
    DynamicArray *pPlatformArray = createDynamicArray(ARRAY_ENTITIES);

    // Add blocks along the bottom of the screen.
    int windowHeight = (float)MAX_LOGICAL_WIDTH * mainDisplay.h/mainDisplay.w;
    for(int i = 0; i < MAX_LOGICAL_WIDTH; i+=32) {
        addEntity(pPlatformArray, i, windowHeight-32, pGrassTexture, HITBOX_FULL_BLOCK);
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
    addEntity(pPlatformArray, MAX_LOGICAL_WIDTH-32, 32, pGrassTexture, HITBOX_FULL_BLOCK);

    Camera *pCamera = createCamera(mainDisplay.w, mainDisplay.h, mainDisplay.refresh_rate, SCALING);
    cameraSetRenderer(pCamera, getRenderer(pWindow));
    cameraSetTargets(pCamera, playerGetBody(pPlayer), pMObject);
    cameraSetMode(pCamera, TRACKING_T1);
    cameraSetZoom(pCamera, MAX_ZOOM_IN);

    //Entity *pPlayer = createEntity(createVector(32, 32), pPlayerTexture, HITBOX_PLAYER);


//--------------------------------------------------------------------------------------------------------------------//

    //RotateObject
    //Entity *pMObject = createEntity(createVector(256, 256), pPlayerTexture, 0, HITBOX_FULL_BLOCK);

    int x, y;
    Vec2 mouseVector = createVector(0.0f, 0.0f);
   
//--------------------------------------------------------------------------------------------------------------------//

    bool gameRunning = true;

    SDL_Event event;
 

    const float timestep = 1.0f/60.0f; // Fixed timestep (60 Updates per second)
    Uint32 lastTime = SDL_GetTicks();
    float deltaTime = 0.0f;
    float accumulator = 0.0f;

    bool gameRunning = true;
    while(gameRunning) {
        Uint32 currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) * 0.001f; // ms till sekunder
        lastTime = currentTime;
        accumulator += deltaTime;

        mouseVector = cameraGetMousePosition(pCamera);
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
            
        cameraUpdate(pCamera);

        bool standingOnPlatform = false;
        for (int i = 0; i < arrayGetSize(pPlatformArray); i++) {
            if (playerCheckCollision(pPlayer, arrayGetObject(pPlatformArray, i)) == OBJECT_IS_NORTH) {
                standingOnPlatform = true;
            }
        }

/*         if (playerCheckCollision(pPlayer, playerGetBody(pReference)) == OBJECT_IS_NORTH) {
            standingOnPlatform = true;
        } */


        if (!standingOnPlatform) { playerSetState(pPlayer, FALLING); }

        clearWindow(pWindow);


        renderPlayer(pWindow, pPlayer, pCamera);
        //renderEntity(pWindow, playerGetBody(pReference), pCamera);


        for (int i = 0; i < arrayGetSize(pPlatformArray); i++) {
            Entity *pEntity = arrayGetObject(pPlatformArray, i);
            if (entityIsVisible(pCamera, getCurrentFrame(pEntity))) {
                renderEntity(pWindow, pEntity, pCamera);
            }
        }

        if(playerGetMouseClick(pPlayer)) {
            drawLine(pWindow, mouseVector, getMidPoint(playerGetBody(pPlayer)));
        } 

        //renderEntity(pWindow, pMObject);

        displayWindow(pWindow);
    }
    
    destroyPlayer(pPlayer);
    //destroyPlayer(pReference);
    destroyDynamicArray(pPlatformArray);
    SDL_DestroyTexture(pPlayerTexture);
    SDL_DestroyTexture(pGrassTexture);
    destroyCamera(pCamera);
    destroyRenderWindow(pWindow);
    SDL_Quit();

    return 0;
}