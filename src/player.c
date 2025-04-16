#include "player.h"

#define JUMP_TIMER 10

typedef enum {
    IDLE, RUNNING, JUMPING, FALLING, ROTATING, FLYING
} PlayerState;

typedef enum {
    NEUTRAL, LEFT, RIGHT, UP, DOWN, BLOCKED
} Direction;

struct player {
    Entity *pBody;
    Entity *pTongue;
    PlayerState state;
    Direction directionX;
    Direction directionY;
    float gravityModifier;
    int jumpTimer;
};

Player *createPlayer(Vec2 position, SDL_Texture *pTexture) {
    Player *pPlayer = malloc(sizeof(Player));
    pPlayer->pBody = createEntity(position, pTexture, HITBOX_PLAYER);
    pPlayer->pTongue = NULL;
    pPlayer->state = FALLING;
    pPlayer->directionX = NEUTRAL;
    pPlayer->directionY = NEUTRAL;
    pPlayer->gravityModifier = 0.0f;
    pPlayer->jumpTimer = 0;

    return pPlayer;
}

bool playerHandleInput(Player *pPlayer) {
    bool gameRunning = true;
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            gameRunning = false;
        }
        if(event.type == SDL_KEYDOWN) {
            switch(event.key.keysym.scancode) {
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
                        default:
                            pPlayer->state = FLYING;
                    }
                    break;
                case SDL_SCANCODE_SPACE:
                    switch (pPlayer->state) {
                        case IDLE:
                            pPlayer->state = JUMPING;
                            break;
                    }
                    break;
            }
        }
        else if(event.type == SDL_KEYUP) {
            switch(event.key.keysym.scancode) {
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
            }
        }
    }

    return gameRunning;
}

void playerUpdateState(Player *pPlayer, float deltaTime) {
    switch (pPlayer->state) {
        case IDLE:
        case RUNNING:
            pPlayer->gravityModifier = 0.0f;
            setVelocityY(pPlayer->pBody, 0.0f);
            break;
        case FALLING:
            if (pPlayer->jumpTimer > 0) {
                setVelocityY(pPlayer->pBody, 0.0f);
                pPlayer->jumpTimer = 0;
            }
            pPlayer->gravityModifier = 1.0f;
            break;
        case JUMPING:
            if (pPlayer->jumpTimer == 0) { 
                pPlayer->jumpTimer = JUMP_TIMER;
                pPlayer->gravityModifier = 0.5f;
                setVelocityY(pPlayer->pBody, -JUMP_VELOCITY);
            }
            else if (pPlayer->jumpTimer > 0) {
                pPlayer->jumpTimer--;
                if (pPlayer->jumpTimer == 0) { pPlayer->state = FALLING; }
            }
            break;
        case FLYING:
            pPlayer->gravityModifier = 0.0f;
            break;
    }

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
        case UP:
            if (getVelocity(pPlayer->pBody).x >= 0.0f) {
                setVelocityX(pPlayer->pBody, -MAX_PLAYER_VELOCITY*0.75f);
            }
            break;
        case DOWN:
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

void playerUpdatePosition(Player *pPlayer, float deltaTime) {
    switch (pPlayer->state) {
        case FALLING:
        case JUMPING:
        case IDLE:
        case RUNNING:
            standardCalculations(pPlayer);
            updatePosition(pPlayer->pBody, deltaTime);
            break;
        case ROTATING:
            break;
        case FLYING:
            flyingCalculations(pPlayer);
            updatePosition(pPlayer->pBody, deltaTime);
            break;
    }

    return;
}

bool playerCheckCollision(Player *pPlayer, Entity *pEntity) {
    bool collisionDetected = false;
    Hitbox *pPlayerHitbox = getHitbox(pPlayer->pBody);
    Hitbox *pEntityHitbox = getHitbox(pEntity);
    if (checkCollision(pPlayerHitbox, pEntityHitbox)) {
        Vec2 correction = rectVsRect(pPlayerHitbox, pEntityHitbox);
        collisionResponse(pPlayer->pBody, correction);
        switch (hitboxOrientation(pPlayerHitbox, pEntityHitbox)) {
            case OBJECT_IS_NORTH:
                switch (pPlayer->state) {
                    case FLYING:
                        break;
                    case JUMPING:
                        break;
                    default:
                        pPlayer->state = IDLE;
                        break;
                }
                collisionDetected = true;
                break;
            case OBJECT_IS_SOUTH:
                break;
            case OBJECT_IS_WEST:
                break;
            case OBJECT_IS_EAST:
                break;
        }
    }

    return collisionDetected;
}

void playerSetFalling(Player *pPlayer) {
    if (pPlayer->state != JUMPING) {
        pPlayer->state = FALLING;
    }

    return;
}

Entity *playerGetBody(Player const *pPlayer) {
    return pPlayer->pBody;
}

void destroyPlayer(Player *pPlayer) {
    if (pPlayer == NULL) { return; }

    destroyEntity(pPlayer->pBody);
    destroyEntity(pPlayer->pTongue);
    free(pPlayer);
    return;
}