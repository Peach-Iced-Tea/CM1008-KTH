#include "main.h"

void cleanUp(RenderWindow *pRW, SDL_Texture *pT, Player *pP1, Player *pP2, DynamicArray *pDA, Camera *pC, Input *pI) {
    destroyInputTracker(pI);
    destroyPlayer(pP1);
    destroyPlayer(pP2);
    destroyDynamicArray(pDA);
    SDL_DestroyTexture(pT);
    destroyCamera(pC);
    destroyRenderWindow(pRW);
    SDL_Quit();
    return;
}

int main(int argv, char** args) {
    if (SDL_Init(SDL_INIT_VIDEO)!=0) {
        printf("Error: %s\n",SDL_GetError());
        return 1;
    }

    // Get information about the main display, such as pixel width and pixel height.
    SDL_DisplayMode mainDisplay;
    SDL_GetDesktopDisplayMode(0, &mainDisplay);

    RenderWindow *pWindow = createRenderWindow("ToTheTop", mainDisplay.w, mainDisplay.h);
    if (pWindow == NULL) { SDL_Quit(); return 1; }

    SDL_Texture *pGrassTexture = loadTexture(pWindow, "resources/purpg.png");
    if (pGrassTexture == NULL) { cleanUp(pWindow, NULL, NULL, NULL, NULL, NULL, NULL); return 1; }

    Player *pPlayer = createPlayer(createVector(32.0f, 32.0f), getRenderer(pWindow), PLAYER_1);
    if (pPlayer == NULL) { cleanUp(pWindow, pGrassTexture, NULL, NULL, NULL, NULL, NULL); return 1; }

    Player *pReference = createPlayer(createVector(256.0f, 256.0f), getRenderer(pWindow), PLAYER_2);
    if (pReference == NULL) { cleanUp(pWindow, pGrassTexture, pPlayer, NULL, NULL, NULL, NULL); return 1; }

    DynamicArray *pPlatformArray = createDynamicArray(ARRAY_ENTITIES);
    if (pPlatformArray == NULL) { cleanUp(pWindow, pGrassTexture, pPlayer, pReference, NULL, NULL, NULL); return 1; }
    // Add blocks along the bottom of the screen.
    for(int i = 0; i < REFERENCE_WIDTH; i+=32) {
        addEntity(pPlatformArray, i, REFERENCE_HEIGHT-32, pGrassTexture, HITBOX_FULL_BLOCK);
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
    addEntity(pPlatformArray, REFERENCE_WIDTH-32, 32, pGrassTexture, HITBOX_FULL_BLOCK);

//--------------------------------------------------------------------------------------------------------------------//
    Camera *pCamera = createCamera(mainDisplay.w, mainDisplay.h, mainDisplay.refresh_rate, SCALING);
    if (pCamera == NULL) { cleanUp(pWindow, pGrassTexture, pPlayer, pReference, pPlatformArray, NULL, NULL); return 1; }

    cameraSetRenderer(pCamera, getRenderer(pWindow));
    cameraSetTargets(pCamera, playerGetBody(pPlayer), playerGetBody(pReference));

    Input *pInputs = createInputTracker();
    if (pInputs == NULL) { cleanUp(pWindow, pGrassTexture, pPlayer, pReference, pPlatformArray, pCamera, NULL); return 1; }
//--------------------------------------------------------------------------------------------------------------------//
 
    const float timestep = 1.0f/60.0f; // Fixed timestep (60 Updates per second)
    Uint32 lastTime = SDL_GetTicks();
    float deltaTime = 0.0f;
    float accumulator = 0.0f;

    bool gameRunning = true;
    while (gameRunning) {
        Uint32 currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) * 0.001f; // ms till sekunder
        lastTime = currentTime;
        accumulator += deltaTime;

        Vec2 mousePosition = cameraGetMousePosition(pCamera);

        gameRunning = checkUserInput(pInputs);
        playerHandleInput(pPlayer, pInputs);
        cameraHandleInput(pCamera, pInputs);
        windowHandleInput(pWindow, pInputs);

        if (playerGetMouseClick(pPlayer)) {
            Entity *pBodyP1 = playerGetBody(pPlayer);
            if (vectorLength(mousePosition, getMidPoint(pBodyP1)) < 240.0f) {
                Entity *pBodyP2 = playerGetBody(pReference);
                if (pointVsRect(getHitbox(pBodyP2), mousePosition)) {
                    playerSetState(pPlayer, ROTATING);
                    float radius = vectorLength(getMidPoint(pBodyP1), getMidPoint(pBodyP2));
                    float alpha = vectorGetAngle(getMidPoint(pBodyP1), getMidPoint(pBodyP2));
                    playerSetRadius(pPlayer, radius);
                    playerSetReferenceAngle(pPlayer, alpha);
                }
            }
        }

        if (accumulator >= timestep) {    
            // Add physics related calculations here...
            inputHoldTimer(pInputs);
            playerUpdateState(pPlayer, timestep);
            accumulator -= timestep;
        }

        switch (playerGetState(pPlayer)) {
            case ROTATING:
                Entity *pBodyP2 = playerGetBody(pReference);
                Vec2 rotateVelocity;
                Vec2 newRotPos = playerUpdatePosition(pPlayer, deltaTime);
                vectorSub(&rotateVelocity, newRotPos, getMidPoint(pBodyP2));
                setVelocityX(pBodyP2, rotateVelocity.x);
                setVelocityY(pBodyP2, rotateVelocity.y);
                updatePosition(pBodyP2, 1.0f);
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

        if (playerCheckCollision(pPlayer, playerGetBody(pReference)) == OBJECT_IS_NORTH) {
            standingOnPlatform = true;
        }

        if (!standingOnPlatform) { playerSetState(pPlayer, FALLING); }

        cameraUpdate(pCamera);

        clearWindow(pWindow);

        renderPlayer(pWindow, pPlayer, pCamera);
        renderPlayer(pWindow, pReference, pCamera);
        for (int i = 0; i < arrayGetSize(pPlatformArray); i++) {
            Entity *pEntity = arrayGetObject(pPlatformArray, i);
            if (entityIsVisible(pCamera, getCurrentFrame(pEntity))) {
                renderEntity(pWindow, pEntity, pCamera);
            }
        }

        if (playerGetMouseClick(pPlayer)) {
            drawLine(pWindow, mousePosition, getMidPoint(playerGetBody(pPlayer)), pCamera);
        }
        
        displayWindow(pWindow);
    }

    cleanUp(pWindow, pGrassTexture, pPlayer, pReference, pPlatformArray, pCamera, pInputs);

    return 0;
}
