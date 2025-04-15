#include "main.h"

int main(int argv, char** args) {
    if(SDL_Init(SDL_INIT_VIDEO)!=0) {
        printf("Error: %s\n",SDL_GetError());
        return 1;
    }

    // Get information about the main display, such as pixel width and pixel height.
    SDL_DisplayMode mainDisplay;
    SDL_GetDesktopDisplayMode(0, &mainDisplay);
    int width = mainDisplay.w;
    int height = mainDisplay.h;

    Camera *pCamera = createCamera(width, height, mainDisplay.refresh_rate, CAMERA_SCALING);
    // rendera ett fönster
    RenderWindow *pWindow = createRenderWindow("ToTheTop", width, height);
    // rendera in Texturer för sig
    SDL_Texture *pGrassTexture = loadTexture(pWindow, "resources/purpg.png");
    // laddar Entitys med pekare till texturer
    SDL_Texture *pPlayerTexture = loadTexture(pWindow, "resources/player1.png");

    Player *player = createPlayer(createVector(32.0f, 32.0f), pPlayerTexture);

    // DynamicArray
    DynamicArray *pPlatformArray = createDynamicArray(ARRAY_ENTITIES);
    // Add blocks along the top of the screen.
    //for(int i = 0; i < (WINDOW_W/(32*GLOBAL_SCALER) * 32); i+=32) {
    //    addEntity(&platformArray, i, 0.0f, grassTexture, HITBOX_FULL_BLOCK);
    //}

    // Add blocks along the middle of the y-axis.
    /*for(int i = 0; i < (WINDOW_W/(32*GLOBAL_SCALER) * 32); i+=32) {
        addEntity(&platformArray, i, (WINDOW_H*0.5f-32*GLOBAL_SCALER*0.5f)/GLOBAL_SCALER, grassTexture, HITBOX_FULL_BLOCK);
    }*/

    // Add blocks along the bottom of the screen.
    for(int i = 0; i < width; i+=32) {
        addEntity(pPlatformArray, i, height-32, pGrassTexture, HITBOX_FULL_BLOCK);
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
    addEntity(pPlatformArray, width-32, 32, pGrassTexture, HITBOX_FULL_BLOCK);

    Entity *pReference = createEntity(createVector(128.0f, 32.0f), pPlayerTexture, HITBOX_NONE);
    cameraSetRenderer(pCamera, getRenderer(pWindow));
    cameraSetTargets(pCamera, playerGetBody(player), pReference);

    SDL_Event event;
    Vec2 mousePosition;

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

        mousePosition = cameraGetMousePosition(pCamera);
        /*while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                gameRunning = false;
            }
            else if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.scancode) {
                    case SDL_SCANCODE_ESCAPE:
                        gameRunning = false;
                        break;
                    case SDL_SCANCODE_T:
                        printf("dT: %f\n", deltaTime);
                        printf("MousePos: x=%f, y=%f\n", mousePosition.x, mousePosition.y);
                        //printf("PlayerPos: x=%f, y=%f\n", getPosition(pPlayer).x, getPosition(pPlayer).y);
                        //printf("PlatformPos: x=%f, y=%f\n", getPosition(platformArray.entities[0]).x, getPosition(platformArray.entities[0]).y);
                        //printf("playerHbox: x=%f, y=%f\n", getHitboxPosition(getHitbox(pPlayer)).x, getHitboxPosition(getHitbox(pPlayer)).y);
                        //printf("PlatformHbox: x=%f, y=%f\n", getHitboxPosition(getHitbox(platformArray.entities[0])).x, getHitboxPosition(getHitbox(platformArray.entities[0])).y);
                        printf("PlatformArray.size: %ld\n", arrayGetSize(pPlatformArray));
                        //printf("playerVelocity.x: %f\n", getVelocity(pPlayer).x);
                        //printf("playerVelocity.y: %f\n", getVelocity(pPlayer).y);
                        break;
                }
            }
            else if(event.type == SDL_KEYUP) {
                switch(event.key.keysym.scancode) {
                    case SDL_SCANCODE_RETURN:
                        toggleFullscreen(pWindow);
                        break;
                    case SDL_SCANCODE_1:
                        cameraSetMode(pCamera, CAMERA_SCALING);
                        break;
                    case SDL_SCANCODE_2:
                        cameraSetMode(pCamera, CAMERA_TRACKING_T1);
                        break;
                    case SDL_SCANCODE_3:
                        cameraSetMode(pCamera, CAMERA_TRACKING_T2);
                        break;
                    case SDL_SCANCODE_PERIOD:
                        cameraSetZoom(pCamera, MAX_ZOOM_IN);
                        break;
                    case SDL_SCANCODE_COMMA:
                        cameraSetZoom(pCamera, MAX_ZOOM_OUT);
                        break;
                }
            }
        }*/

        gameRunning = playerHandleInput(player);

        if (accumulator >= timestep) {    
            // Add physics related calculations here...
            playerUpdateState(player, timestep);
            accumulator -= timestep;
        }

        playerUpdatePosition(player, deltaTime);

        bool collisionDetected = false;
        for(int i = 0; i < arrayGetSize(pPlatformArray); i++) {
            if (playerCheckCollision(player, arrayGetObject(pPlatformArray, i))) {
                collisionDetected = true;
            }
        }

        if (!collisionDetected) { playerSetFalling(player); }

        clearWindow(pWindow);
        cameraUpdate(pCamera);
        renderEntity(pWindow, playerGetBody(player), pCamera);
        renderEntity(pWindow, pReference, pCamera);
        for(int i = 0; i < arrayGetSize(pPlatformArray); i++) {
            renderEntity(pWindow, arrayGetObject(pPlatformArray, i), pCamera);
        }
        
        displayWindow(pWindow);
    }
    
    destroyPlayer(player);
    destroyEntity(pReference);
    destroyDynamicArray(pPlatformArray);
    SDL_DestroyTexture(pPlayerTexture);
    SDL_DestroyTexture(pGrassTexture);
    destroyCamera(pCamera);
    destroyRenderWindow(pWindow);
    SDL_Quit();

    return 0;
}