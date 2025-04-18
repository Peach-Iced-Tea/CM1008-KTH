#include "player.h"

#define JUMP_TIMER 10
#define ROTSPEED 100

typedef enum {
    NEUTRAL, LEFT, RIGHT, UP, DOWN, BLOCKED, ROT_RIGHT, ROT_LEFT
} Direction;

struct player {
    Entity *pBody;
    Entity *pTongue;
    PlayerState state;
    Direction directionX;
    Direction directionY;
    Direction rotateDirection;
    bool mouseClicked;
    float gravityModifier;
    int jumpTimer;
    float radius;
    float referenceAngle;
    SDL_Rect sheetPosition;
};

Player *createPlayer(Vec2 position, SDL_Texture *pTexture) {
    Player *pPlayer = malloc(sizeof(Player));
    pPlayer->pBody = createEntity(position, pTexture, ENTITY_PLAYER, HITBOX_PLAYER);
    pPlayer->pTongue = NULL;
    pPlayer->state = FALLING;
    pPlayer->directionX = NEUTRAL;
    pPlayer->directionY = NEUTRAL;
    pPlayer->gravityModifier = 0.0f;
    pPlayer->jumpTimer = 0;
    pPlayer->mouseClicked = false;
    pPlayer->rotateDirection = NEUTRAL;


    pPlayer->sheetPosition.x = 0;
    pPlayer->sheetPosition.y = 0;
    pPlayer->sheetPosition.w = 32;
    pPlayer->sheetPosition.h = 32; 

    return pPlayer;
}

bool playerHandleInput(Player *pPlayer) {
    bool gameRunning = true;
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            gameRunning = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    gameRunning = false;
                    break;
                case SDL_SCANCODE_W:
                    switch (pPlayer->directionY) {
                        case NEUTRAL:
                            pPlayer->directionY = UP;
                            break;
                        case DOWN:
                            pPlayer->directionY = BLOCKED;
                            break;
                    }
                    break;
                case SDL_SCANCODE_S:
                    switch (pPlayer->directionY) {
                        case NEUTRAL:
                            pPlayer->directionY = DOWN;
                            break;
                        case UP:
                            pPlayer->directionY = BLOCKED;
                            break;
                    }
                    break;
                case SDL_SCANCODE_A:
                    switch (pPlayer->directionX) {
                        case NEUTRAL:
                            pPlayer->directionX = LEFT;
                            break;
                        case RIGHT:
                            pPlayer->directionX = BLOCKED;
                            break;
                    }
                    break;
                case SDL_SCANCODE_D:
                    switch (pPlayer->directionX) {
                        case NEUTRAL:
                            pPlayer->directionX = RIGHT;
                            break;
                        case LEFT:
                            pPlayer->directionX = BLOCKED;
                            break;
                    }
                    break;
                case SDL_SCANCODE_G:
                    switch (pPlayer->state) {
                        case FLYING:
                            pPlayer->state = FALLING;
                            break;
                        case FALLING:
                            pPlayer->state = FLYING;
                            break;
                        case IDLE:
                            pPlayer->state = FLYING;
                            break;

                    }
                    break;
                case SDL_SCANCODE_SPACE:
                    switch (pPlayer->state) {
                        case IDLE:
                            pPlayer->state = JUMPING;
                        break;
                    }
                    break;
                case SDL_SCANCODE_E:
                    switch (pPlayer->rotateDirection) {
                        case NEUTRAL:
                            pPlayer->rotateDirection = ROT_RIGHT;
                            break;
                        case ROT_LEFT:
                            pPlayer->rotateDirection = BLOCKED;
                            break;
                    }
                    break;
                case SDL_SCANCODE_Q:
                    switch (pPlayer->rotateDirection) {
                        case NEUTRAL:
                            pPlayer->rotateDirection = ROT_LEFT;
                            break;
                        case ROT_RIGHT:
                            pPlayer->rotateDirection = BLOCKED;
                            break;
                    }
                    break;
            }
        }
        else if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                    switch (pPlayer->directionY) {
                        case BLOCKED:
                            pPlayer->directionY = DOWN;
                            break;
                        case UP:
                            pPlayer->directionY = NEUTRAL;
                            break;
                    }
                    break;
                case SDL_SCANCODE_S:
                    switch (pPlayer->directionY) {
                        case BLOCKED:
                            pPlayer->directionY = UP;
                            break;
                        case DOWN:
                            pPlayer->directionY = NEUTRAL;
                            break;
                    }
                    break;
                case SDL_SCANCODE_A:
                    switch (pPlayer->directionX) {
                        case BLOCKED:
                            pPlayer->directionX = RIGHT;
                            break;
                        case LEFT:
                            pPlayer->directionX = NEUTRAL;
                            break;
                    }
                    break;
                case SDL_SCANCODE_D:
                    switch (pPlayer->directionX) {
                        case BLOCKED:
                            pPlayer->directionX = LEFT;
                            break;
                        case RIGHT:
                            pPlayer->directionX = NEUTRAL;
                            break;
                    }
                    break;
                case SDL_SCANCODE_SPACE:
                    pPlayer->state = FALLING;
                    break;
                case SDL_SCANCODE_E:
                    switch (pPlayer->rotateDirection) {
                        case BLOCKED:
                            pPlayer->rotateDirection = ROT_LEFT;
                            break;
                        case ROT_RIGHT:
                            pPlayer->rotateDirection = NEUTRAL;
                            break;
                    }
                    break;
                case SDL_SCANCODE_Q:
                    switch (pPlayer->rotateDirection) {
                        case BLOCKED:
                            pPlayer->rotateDirection = ROT_RIGHT;
                            break;
                        case ROT_LEFT:
                            pPlayer->rotateDirection = NEUTRAL;
                            break;
                    }
                    break;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    pPlayer->mouseClicked = true;
                    break;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONUP) {
            switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    pPlayer->mouseClicked = false;
                    switch (pPlayer->state) {
                        case ROTATING:
                            pPlayer->state = FALLING;
                            break;
                    }
            }
        }
    }

    return gameRunning;
}

void playerUpdateState(Player *pPlayer, float deltaTime) {
    int offset = 0;
    switch (pPlayer->directionX) {
        case LEFT:
            offset = 32;
            break;
        case RIGHT:
            offset = 0;
            break;
        case NEUTRAL:
        case BLOCKED:
            offset = pPlayer->sheetPosition.x;
            break;
    }
    
    switch (pPlayer->state) {
        case IDLE:
        case RUNNING:
            pPlayer->gravityModifier = 0.0f;
            setVelocityY(pPlayer->pBody, 0.0f);

            pPlayer->sheetPosition.y = 0;
            break;
        case FALLING:
            if (pPlayer->jumpTimer > 0) {
                setVelocityY(pPlayer->pBody, 0.0f);
                pPlayer->jumpTimer = 0;
            }
            pPlayer->gravityModifier = 1.0f;

            pPlayer->sheetPosition.y = 64;
            break;
        case JUMPING:
            if (pPlayer->jumpTimer == 0) {
                pPlayer->jumpTimer = JUMP_TIMER;
                pPlayer->gravityModifier = 0.5f;
                setVelocityY(pPlayer->pBody, -JUMP_VELOCITY);
            }
            else if (pPlayer->jumpTimer > 0) {
                pPlayer->jumpTimer--;
                if(pPlayer->jumpTimer == 0) {pPlayer->state = FALLING;}
            }

            pPlayer->sheetPosition.y = 32;
            break;
        case FLYING:
            pPlayer->gravityModifier = 0.0f;

            pPlayer->sheetPosition.y = 0;
            break;
        case ROTATING:
            pPlayer->gravityModifier = 0.0f;

            offset = pPlayer->sheetPosition.x;
            pPlayer->sheetPosition.y = 0;
            break;
    }
    pPlayer->sheetPosition.x = offset;

    setAccelerationY(pPlayer->pBody, GRAVITY_ACCELERATION*pPlayer->gravityModifier);
    updateVelocity(pPlayer->pBody, deltaTime);
}

void standardCalculations(Player *pPlayer) {
    switch (pPlayer->directionX) {
        case LEFT:
            if (getVelocity(pPlayer->pBody).x >= 0.0f) {
                setVelocityX(pPlayer->pBody, -PLAYER_VELOCITY);
                setAccelerationX(pPlayer->pBody, -PLAYER_ACCELERATION);
            }
            break;
        case RIGHT:
            if (getVelocity(pPlayer->pBody).x <= 0.0f) {
                setVelocityX(pPlayer->pBody, PLAYER_VELOCITY);
                setAccelerationX(pPlayer->pBody, PLAYER_ACCELERATION);
            }
            break;
        case NEUTRAL:
        case BLOCKED:
            setVelocityX(pPlayer->pBody, 0.0f);
            setAccelerationX(pPlayer->pBody, 0.0f);
            break;    
    }

    return;
}

void flyingCalculations(Player *pPlayer) {
    switch (pPlayer->directionX) {
        case LEFT:
            if (getVelocity(pPlayer->pBody).x >= 0.0f) {
                setVelocityX(pPlayer->pBody, -MAX_PLAYER_VELOCITY*0.75f);
            }
            break;
        case RIGHT:
            if (getVelocity(pPlayer->pBody).x <= 0.0f) {
                setVelocityX(pPlayer->pBody, MAX_PLAYER_VELOCITY*0.75f);
            }
            break;
        case NEUTRAL:
        case BLOCKED:
            setVelocityX(pPlayer->pBody, 0.0f);
            setAccelerationX(pPlayer->pBody, 0.0f);
            break;
    }

    switch (pPlayer->directionY) {
        case UP:
            if (getVelocity(pPlayer->pBody).y >= 0.0f) {
                setVelocityY(pPlayer->pBody, -MAX_PLAYER_VELOCITY*0.75f);
            }
            break;
        case DOWN:
            if (getVelocity(pPlayer->pBody).y <= 0.0f) {
                setVelocityY(pPlayer->pBody, MAX_PLAYER_VELOCITY*0.75f);
            }
            break;
        case NEUTRAL:
        case BLOCKED:
            setVelocityY(pPlayer->pBody, 0.0f);
            setAccelerationY(pPlayer->pBody, 0.0f);
            break;
    }

    return;
}

Vec2 rotationCalculations(Player *pPlayer, float deltaTime) {
    Vec2 newPosition;
    switch (pPlayer->rotateDirection) {
        case ROT_LEFT:
            pPlayer->referenceAngle -= (PI/180) * deltaTime * ROTSPEED;
            break;
        case ROT_RIGHT:
            pPlayer->referenceAngle += (PI/180) * deltaTime * ROTSPEED;
            break;
    }   

    newPosition.x =(getMidPoint(pPlayer->pBody).x + cosf(pPlayer->referenceAngle) * pPlayer->radius);
    newPosition.y =(getMidPoint(pPlayer->pBody).y + sinf(pPlayer->referenceAngle) * pPlayer->radius);
    
    return newPosition;
}

Vec2 playerUpdatePosition(Player *pPlayer, float deltaTime) {
    Vec2 returnVector;
    switch(pPlayer->state) {
        case FALLING:
        case JUMPING:
        case IDLE:
        case RUNNING:
            standardCalculations(pPlayer);
            updatePosition(pPlayer->pBody, deltaTime);
            break;
        case ROTATING:
            returnVector = rotationCalculations(pPlayer, deltaTime);
            break;
        case FLYING:
            flyingCalculations(pPlayer);
            updatePosition(pPlayer->pBody, deltaTime);
            break;
    }

    return returnVector;
}

int playerCheckCollision(Player *pPlayer, Entity *pEntity) {
    int collisionDetected = 0;    
    Hitbox *pPlayerHitbox = getHitbox(pPlayer->pBody);
    Hitbox *pEntityHitbox = getHitbox(pEntity);
    if (checkCollision(pPlayerHitbox, pEntityHitbox)) {
        Vec2 correction = rectVsRect(pPlayerHitbox, pEntityHitbox);
        collisionResponse(pPlayer->pBody, correction);
        collisionDetected = hitboxOrientation(pPlayerHitbox, pEntityHitbox);
        switch (collisionDetected) {
            case OBJECT_IS_NORTH:
                playerSetState(pPlayer, IDLE);
                break;
            case OBJECT_IS_SOUTH:
                switch (pPlayer->state) {
                    case JUMPING:
                        pPlayer->state = FALLING;
                        break;
                }
                break;
            case OBJECT_IS_WEST:
                break;
            case OBJECT_IS_EAST:
                break;
        }
    }

    return collisionDetected;
}

bool playerSetState(Player *pPlayer, int newState) {
    bool stateWasChanged = false;
    switch (newState) {
        case IDLE:
            switch (pPlayer->state) {
                case JUMPING:
                case FLYING:
                    break;
                default:
                    stateWasChanged = true;
                    break;
            }
            break;
        case FALLING:
            switch (pPlayer->state) {
                case JUMPING:
                case FLYING:
                case ROTATING:
                    break;
                default:
                    stateWasChanged = true;
                    break;
            }
            break;
        case ROTATING:
            stateWasChanged = true;
        default:
            break;   
    }

    if (stateWasChanged) { pPlayer->state = newState; }

    return stateWasChanged;
}

void playerSetRadius(Player *pPlayer, float radius) {
    if (radius <= 0.0f) {
        return;
    }
    pPlayer->radius = radius;
}

void playerSetReferenceAngle(Player *pPlayer, float newAngle) {
    pPlayer->referenceAngle = newAngle;
}

int playerGetState(Player *pPlayer) {
    return pPlayer->state;
}

Entity *playerGetBody(Player const *pPlayer) {
    return pPlayer->pBody;
}

SDL_Rect playerGetSheetPosition(Player *pPlayer) {
    return pPlayer->sheetPosition;
}

bool playerGetMouseClick(Player const *pPlayer) {
    return pPlayer->mouseClicked;
}

void destroyPlayer(Player *pPlayer) {
    if (pPlayer == NULL) { return; }

    destroyEntity(pPlayer->pBody);
    destroyEntity(pPlayer->pTongue);
    free(pPlayer);
    return;
}