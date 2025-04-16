#include "main.h"

// Change GLOBAL_SCALER inside renderWindow.h to change the scaling of the window.
#define WINDOW_W 1920
#define WINDOW_H 1080

#define ROTSPEED 100

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
    bool clique = false;



    bool rotation = false;
    float radius = 0.0f;

    float alpha = 0.0f;
    float newAlpha = 0.0f;

    Vec2 rotateVelocity = createVector(0.0f, 0.0f);
    Vec2 newRotPos = createVector(0.0f, 0.0f);

    bool rotateRight = false;
    bool rotateLeft = false;


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

/*
        currentDirection.x = 0.0f;
        currentDirection.y = 0.0f;
        while(SDL_PollEvent(&event)) {  // Turn this into a function, maybe add a struct containing a Vec2* and some other variables.
            if(event.type == SDL_QUIT) {
                gameRunning = false;
            }
            else if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.scancode) {
                    case SDL_SCANCODE_ESCAPE:
                        gameRunning = false;
                        break;
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
                        printf("PlayerPos: x=%f, y=%f\n", getPosition(pPlayer).x, getPosition(pPlayer).y);
                        //printf("PlatformPos: x=%f, y=%f\n", getPosition(platformArray.entities[0]).x, getPosition(platformArray.entities[0]).y);
                        printf("playerHbox: x=%f, y=%f\n", getHitboxPosition(getHitbox(pPlayer)).x , getHitboxPosition(getHitbox(pPlayer)).y);
                        //printf("PlatformHbox: x=%f, y=%f\n", getHitboxPosition(getHitbox(platformArray.entities[0])).x, getHitboxPosition(getHitbox(platformArray.entities[0])).y);
                        printf("PlatformArray.size: %ld\n", arrayGetSize(pPlatformArray));
                        printf("playerVelocity.x: %f\n", getVelocity(pPlayer).x);
                        printf("playerVelocity.y: %f\n", getVelocity(pPlayer).y);
                        printf("Distans_PlayerToObject: %f\n", vectorLength(getMidPoint(pPlayer), getMidPoint(pMObject)));
                        printf("Alpha: %f\n", alpha);
                        printf("Cos(3PI/2): %f\nSin(3PI/2): %f\n", cosf(3*PI*0.5f), sinf(3*PI*0.5f));
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
                    case SDL_SCANCODE_E:
                        if(rotation) {
                            rotateRight = true;
                        }   
                        break;
                    case SDL_SCANCODE_Q:
                        if(rotation) {
                            rotateLeft = true;
                        }   
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
                    case SDL_SCANCODE_E:
                        rotateRight = false;
                        break;
                    case SDL_SCANCODE_Q:
                        rotateLeft = false;
                        break;
                }
            }
            else if(event.type == SDL_MOUSEBUTTONDOWN){
                switch (event.button.button)
                {
                case SDL_BUTTON_LEFT:
                    clique = true;
                    printf("-----MouseValues:\n");
                    printf("mousex: %f, mousey: %f\n", mouseVector.x, mouseVector.y);
                    printf("midPointX: %f, midPointY: %f\n", getMidPoint(pPlayer).x, getMidPoint(pPlayer).y);
                    printf("Distans_Mouse->PlayerMid: %f\n",  vectorLength(getMidPoint(pPlayer), mouseVector));
                    
                    break;
                }
            }
            else if(event.type == SDL_MOUSEBUTTONUP){
                switch (event.button.button)
                {
                case SDL_BUTTON_LEFT:
                    clique = false;
                    if(rotation){
                        rotation = false;
                        applyVelocity = true;
                    }
                    break;
                }
            }
        }
*/
       

    
        if (playerGetMouseState(pPlayer)) {
            if (vectorLength(mouseVector, getMidPoint(playerGetBody(pPlayer))) < 240.0f) {
                if (touching(getHitbox(pMObject), mouseVector)) {
                    playerSetState(pPlayer, ROTATING); //LOGIK I PLAYER.C 
                }
            }
        }



       /*  if(vectorLength(mouseVector, getMidPoint(pPlayer)) < 240.0f) {
            if(clique && touching(getHitbox(pMObject), mouseVector) && !rotation) {
                //printf("TOUCHING!\n");
                rotation = true; //låsa movement!
                applyVelocity = true;
            }
        } */
        if(rotation){
            gravityModifier = 0.0f;
            
            radius = vectorLength(getMidPoint(pPlayer), getMidPoint(pMObject));
            alpha = vectorGetAngle(getMidPoint(pPlayer), getMidPoint(pMObject));

            if(rotateRight && !rotateLeft) {
                newAlpha = alpha + (((PI/180) * deltaTime) * ROTSPEED);
                //printf("newAlpa: %f\n", newAlpha);


                newRotPos.x = (getMidPoint(playerGetBody(pPlayer)).x + (cosf(newAlpha) * radius));
                newRotPos.y = (getMidPoint(pPlayer).y + (sinf(newAlpha) * radius));
                //setPosition(pMObject, newRotPos);

                vectorSub(&rotateVelocity, newRotPos, getMidPoint(pMObject));


                setVelocityX(pMObject, rotateVelocity.x);
                setVelocityY(pMObject, rotateVelocity.y);
                
                updatePosition(pMObject, 1.0f);
            }
            if(rotateLeft && !rotateRight) {
                newAlpha = alpha - (((PI/180) * deltaTime) * ROTSPEED);
                //printf("newAlpa: %f\n", newAlpha);

                newRotPos.x = (getMidPoint(pPlayer).x + (cosf(newAlpha) * radius));
                newRotPos.y = (getMidPoint(pPlayer).y + (sinf(newAlpha) * radius));
                //setPosition(pMObject, newRotPos);

                vectorSub(&rotateVelocity, newRotPos, getMidPoint(pMObject));


                setVelocityX(pMObject, rotateVelocity.x);
                setVelocityY(pMObject, rotateVelocity.y);
                
                updatePosition(pMObject, 1.0f);
            }

        }




        if (up && godMode && !rotation) { currentDirection.y = -1.0f; }
        if (down && godMode && !rotation) { currentDirection.y += 1.0f; }
        if (left && !rotation) { currentDirection.x = -1.0f; }
        if (right && !rotation) { currentDirection.x += 1.0f; }


        




        if (jump && gravityModifier == 0.0f && !rotation) {
            jumpTimer = 10;
            gravityModifier = 0.50f;
            setVelocityY(pPlayer, -JUMP_VELOCITY);
        }
        else if (!jump && jumpTimer > 0) {
            jumpTimer = 0;
            gravityModifier = 1.0f;
            if (getVelocity(pPlayer).y<=0.0f) { setVelocityY(pPlayer, 0.0f); }
        }

        if (applyVelocity) {
            if (godMode) {
                setVelocityY(pPlayer, currentDirection.y*MAX_PLAYER_VELOCITY*0.75f);
            }

            setVelocityX(pPlayer, currentDirection.x*(PLAYER_VELOCITY*!godMode + MAX_PLAYER_VELOCITY*0.75f*godMode));
            setAccelerationX(pPlayer, currentDirection.x*PLAYER_ACCELERATION*!godMode);

            applyVelocity = false;
        }

        if (accumulator >= timestep) {    
            // Add physics related calculations here...
            if (jumpTimer > 0) {
                jumpTimer--;
                if (jumpTimer == 0) { gravityModifier = 1.0f; }
            }

            if (!godMode) {
                if (gravityModifier == 0.0f) { setVelocityY(pPlayer, 0.0f); }
                float gravity = GRAVITY_ACCELERATION*gravityModifier;
                if (getAcceleration(pPlayer).y != gravity) {
                    setAccelerationY(pPlayer, gravity);
                }
            }

            updateVelocity(pPlayer, timestep);
            accumulator -= timestep;
        }

        updatePosition(pPlayer, deltaTime);
        if (!godMode && jumpTimer == 0) { gravityModifier = 1.0f; }

        Hitbox *pPlayerHitbox = getHitbox(pPlayer);
        for(int i = 0; i < arrayGetSize(pPlatformArray); i++) {
            Hitbox *pEntityHitbox = getHitbox(arrayGetObject(pPlatformArray, i));
            if (checkCollision(pPlayerHitbox, pEntityHitbox)) {
                Vec2 correction = rectVsRect(pPlayerHitbox, pEntityHitbox);
                collisionResponse(pPlayer, correction);
                switch (hitboxOrientation(pPlayerHitbox, pEntityHitbox)) {
                    case OBJECT_IS_NORTH:
                        gravityModifier = 0.0f;
                        break;
                    case OBJECT_IS_SOUTH:
                        break;
                    case OBJECT_IS_WEST:
                        break;
                    case OBJECT_IS_EAST:
                        break;
                }
            }
        }

        clearWindow(pWindow);
        if(clique == true) {
            drawLine(pWindow, mouseVector, getMidPoint(pPlayer));
        } 
        renderEntity(pWindow, pPlayer);
        renderEntity(pWindow, pMObject);
        for(int i = 0; i < arrayGetSize(pPlatformArray); i++) {
            renderEntity(pWindow, arrayGetObject(pPlatformArray, i));
        }
        
        displayWindow(pWindow);
    }
    
    destroyEntity(pPlayer);
    destroyDynamicArray(pPlatformArray);
    SDL_DestroyTexture(pPlayerTexture);
    SDL_DestroyTexture(pGrassTexture);
    destroyRenderWindow(pWindow);
    SDL_Quit();

    return 0;
}