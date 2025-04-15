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
    Entity *pTongue;
    State state;
    Direction direction;
    float gravityModifier;
    int jumpTimer;
};

Player *createPlayer(Vec2 position, SDL_Texture *pTexture) {
    Player *pPlayer = malloc(sizeof(Player));
    pPlayer->pBody = createEntity(position, pTexture, HITBOX_PLAYER);
    pPlayer->pTongue = NULL;
    pPlayer->state = FALLING;
    pPlayer->direction = NEUTRAL;
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
                    //pPlayer->velocity.y -= MAX_PLAYER_VELOCITY*0.75f;
                    break;
                case SDL_SCANCODE_S:
                    //pPlayer->velocity.y += MAX_PLAYER_VELOCITY*0.75f;
                    break;
                case SDL_SCANCODE_A:
                    switch (pPlayer->direction) {
                        case NEUTRAL:
                            pPlayer->direction = LEFT;
                            break;
                        case RIGHT:
                            pPlayer->direction = BLOCKED;
                            break;
                    }
                    break;
                case SDL_SCANCODE_D:
                    if (pPlayer->direction == NEUTRAL) { pPlayer->direction = RIGHT; }
                    else if (pPlayer->direction == LEFT) { pPlayer->direction = BLOCKED; }
                    break;
                case SDL_SCANCODE_G:
                    switch (pPlayer->direction) {
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
                    
                    break;
                case SDL_SCANCODE_S:
                    
                    break;
                case SDL_SCANCODE_A:
                    if (pPlayer->direction == BLOCKED) { pPlayer->direction = RIGHT; }
                    else if (pPlayer->direction == LEFT) { pPlayer->direction = NEUTRAL; }
                    break;
                case SDL_SCANCODE_D:
                    if (pPlayer->direction == BLOCKED) { pPlayer->direction = LEFT; }
                    else if (pPlayer->direction == RIGHT) { pPlayer->direction = NEUTRAL; }
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
    switch (pPlayer->direction) {
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

void playerUpdatePosition(Player *pPlayer, float deltaTime) {
    switch (pPlayer->state) {
        case FALLING:
        case JUMPING:
        case IDLE:
            standardCalculations(pPlayer);
            updatePosition(pPlayer->pBody, deltaTime);
            break;
        case ROTATING:
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
                if (pPlayer->state != JUMPING) { pPlayer->state = IDLE; }
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