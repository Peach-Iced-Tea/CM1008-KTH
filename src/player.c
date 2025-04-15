#include "player.h"

#define JUMP_TIMER 10

typedef enum {
    IDLE, JUMPING, FALLING, ROTATING, FLYING
} State;

typedef enum {
    LEFT, RIGHT, NEUTRAL, BLOCKED
} Direction;


struct player {
    Entity *pBody;
    Entity *pTounge;
    State state;
    Direction direction;
    float gravityModifier;
    int jumpTimer;
};


Player *createPlayer(Vec2 position, SDL_Texture *pTexture) {
    Player *pPlayer = malloc(sizeof(Player));
    pPlayer->pBody = createEntity(position, pTexture, HITBOX_PLAYER);
    pPlayer->pTounge = NULL;
    pPlayer->state = FALLING;
    pPlayer->direction = NEUTRAL;
    pPlayer->gravityModifier = 0.0f;
    pPlayer->jumpTimer = 0;

    return pPlayer;
}


bool playerHandleInput(Player *pPlayer) {
    bool gameRunning = true;
    SDL_Event event;
    while(SDL_PollEvent) {
        if(event.type == SDL_Quit) {
            gameRunning = false;
        }
        if(event.type == SDL_KEYDOWN) {
            switch(event.key.keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    gameRunning = false;
                    break;
                case SDL_SCANCODE_W:
                    break;
                case SDL_SCANCODE_S:
                    break;
                case SDL_SCANCODE_A:
                    switch(pPlayer->direction) {
                        case NEUTRAL:
                            pPlayer->direction = LEFT;
                            break;
                        case RIGHT:
                            pPlayer->direction = BLOCKED;
                            break;
                    }
                    break;
                case SDL_SCANCODE_D:
                    if(pPlayer->direction == NEUTRAL) {pPlayer->direction = RIGHT;}
                    else if(pPlayer->direction == LEFT) {pPlayer->direction = BLOCKED;}
                    break;
                case SDL_SCANCODE_G:
                    switch(pPlayer->direction) {
                        case FLYING:
                            pPlayer->state = FALLING;
                            break;
                        default:
                            pPlayer->state = FLYING;
                    }
                    break;
                case SDL_SCANCODE_SPACE:
                    switch(pPlayer->state) {
                        pPlayer->state = JUMPING;
                        break;
                    }
                    break;
            }
        }
        else if(event.type == SDL_KEYUP) {
            switch(event.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                    break;
                case SDL_SCANCODE_S:
                    break;
                case SDL_SCANCODE_A:
                    if(pPlayer->direction == BLOCKED) {pPlayer->direction = RIGHT;}
                    else if(pPlayer->direction == RIGHT) {pPlayer->direction = NEUTRAL;}
                    break;
                case SDL_SCANCODE_D:
                    if(pPlayer->direction == BLOCKED) {pPlayer->direction = LEFT;}
                    else if(pPlayer->direction == RIGHT) {pPlayer->direction = NEUTRAL;}
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
    switch(pPlayer->state) {
        case IDLE:
            pPlayer->gravityModifier = 0.0f;
            setVelocityY(pPlayer->pBody, 0.0f);
            break;
        case FALLING:
            if(pPlayer->jumpTimer > 0) {
                setVelocityY(pPlayer->pBody, 0.0f);
                pPlayer->jumpTimer = 0;
            }
            pPlayer->gravityModifier = 1.0f;
            break;
        case JUMPING:
            if(pPlayer->jumpTimer == 0) {
                pPlayer->jumpTimer = JUMP_TIMER;
                pPlayer->gravityModifier = 0.5f;
                setVelocityY(pPlayer->pBody, -JUMP_VELOCITY);
            }
            else if(pPlayer->jumpTimer > 0) {
                pPlayer->jumpTimer--;
                if(pPlayer->jumpTimer == 0) {pPlayer->state = FALLING;}
            }
            break;
        case FLYING:
            pPlayer->gravityModifier = 0.0f;
            break;
    }
    setAccelerationY(pPlayer->pBody, GRAVITY_ACCELERATION*pPlayer->gravityModifier);
    updateVelocity(pPlayer->pBody, deltaTime);
}


void standardCalculations();

void playerUpdatePosition(Player *pPlayer, float deltaTime) {
    switch(pPlayer->state) {
        case FALLING:
        case JUMPING:
        case IDLE:
            standardCalculations();
            updatePosition(pPlayer->pBody, deltaTime);
            break;
        case ROTATING:
            break;
    }
    return;
}
