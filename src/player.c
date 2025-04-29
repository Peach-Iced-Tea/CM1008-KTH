#include "player.h"

#define JUMP_TIMER 15
#define PLAYER_VELOCITY 300.0f   // Initial velocity to be applied on a player upon moving.
#define PLAYER_ACCELERATION 50.0f  // A constant for the acceleration of a player, this value gets modified by deltaTime before updating velocity.
#define JUMP_VELOCITY 780.0f    // The velocity to be applied when a player presses jump, add negative sign before this value.
#define ROTATION_SPEED 100

#define MOVE_UP_INPUT KEY_W
#define MOVE_DOWN_INPUT KEY_S
#define MOVE_LEFT_INPUT KEY_A
#define MOVE_RIGHT_INPUT KEY_D
#define JUMP_INPUT KEY_SPACE
#define ROTATE_LEFT_INPUT KEY_Q
#define ROTATE_RIGHT_INPUT KEY_E
#define TOGGLE_GODMODE_INPUT KEY_G

struct player {
    Entity *pBody;
    Tongue *pTongue;
    PlayerState state;
    Vec2 velocity;
    float rotateVelocity;
    float gravityModifier;
    int jumpTimer;
    float radius;
    float referenceAngle;
    SDL_Rect sheetPosition;
};

Player *createPlayer(Vec2 position, SDL_Renderer *pRenderer, int id) {
    if (id < 0 || id > 1) { return NULL; }

    Player *pPlayer = malloc(sizeof(Player));
    SDL_Texture *pTexture;
    if (pRenderer != NULL) {
        switch (id) {
            case PLAYER_1:
                pTexture = IMG_LoadTexture(pRenderer, "resources/spriteSheetPlayer.png");
                break;
            case PLAYER_2:
                pTexture = IMG_LoadTexture(pRenderer, "resources/spriteSheetPlayer.png");
                break;
        }

        if (pTexture == NULL) {
            printf("Error: %s\n", SDL_GetError());
            return NULL;
        }
    }

    pPlayer->pBody = createEntity(position, pTexture, ENTITY_PLAYER, HITBOX_PLAYER);

    pTexture = IMG_LoadTexture(pRenderer, "resources/tongueTip.png");
    pPlayer->pTongue = createTongue(entityGetMidPoint(pPlayer->pBody), pRenderer);

    pPlayer->state = IDLE;
    pPlayer->velocity = createVector(0.0f, 0.0f);
    pPlayer->rotateVelocity = 0.0f;

    pPlayer->gravityModifier = 0.0f;
    pPlayer->jumpTimer = 0;
    pPlayer->radius = 0.0f;
    pPlayer->referenceAngle = 0.0f;

    pPlayer->sheetPosition.x = 0;
    pPlayer->sheetPosition.y = 0;
    pPlayer->sheetPosition.w = 32;
    pPlayer->sheetPosition.h = 32;

    return pPlayer;
}

void playerHandleInput(Player *pPlayer, Input const *pInputs) {
    Vec2 movement = createVector(0.0f, 0.0f);
    switch (pPlayer->state) {
        case SHOOTING:
        case RELEASE:
        case ROTATING:
            break;
        default:
            if (getKeyState(pInputs, MOVE_UP_INPUT)) { movement.y = -PLAYER_VELOCITY; }

            if (getKeyState(pInputs, MOVE_DOWN_INPUT)) { movement.y += PLAYER_VELOCITY; }
        
            if (getKeyState(pInputs, MOVE_LEFT_INPUT)) { movement.x = -PLAYER_VELOCITY; }
        
            if (getKeyState(pInputs, MOVE_RIGHT_INPUT)) { movement.x += PLAYER_VELOCITY; }
    }
    

    if (getKeyState(pInputs, JUMP_INPUT)) {
        switch (pPlayer->state) {
            case IDLE:
            case RUNNING:
                pPlayer->velocity.y = -JUMP_VELOCITY;
                pPlayer->jumpTimer = JUMP_TIMER;
                pPlayer->state = JUMPING;
                break;
        }
    }
    else {
        switch (pPlayer->state) {
            case JUMPING:
                pPlayer->state = FALLING;
                break;
        }
    }

    if (movement.x < 0.0f || movement.x > 0.0f) {
        switch (pPlayer->state) {
            case IDLE:
                pPlayer->state = RUNNING;
        }
    }

    pPlayer->velocity.x = movement.x;
    switch (pPlayer->state) {
        case FALLING:
        case JUMPING:
            break;
        default:
            pPlayer->velocity.y = movement.y;
            break;
    }

    pPlayer->rotateVelocity = 0.0f;
    if (getKeyState(pInputs, ROTATE_LEFT_INPUT)) {
        pPlayer->rotateVelocity = -ROTATION_SPEED;
    }

    if (getKeyState(pInputs, ROTATE_RIGHT_INPUT)) {
        pPlayer->rotateVelocity += ROTATION_SPEED;
    }

    if (getMouseState(pInputs, MOUSE_LEFT)) {
        switch (pPlayer->state) {
            case IDLE:
                pPlayer->state = SHOOTING;
                pPlayer->referenceAngle = vectorGetAngle(entityGetMidPoint(pPlayer->pBody), tongueGetMousePosition(pPlayer->pTongue));
                tongueSetVelocity(pPlayer->pTongue, entityGetMidPoint(pPlayer->pBody));
                break;
        }
    }
    else if (!getMouseState(pInputs, MOUSE_LEFT)) {
        switch (pPlayer->state) {
            case SHOOTING:
            case ROTATING:
                tongueSetVelocity(pPlayer->pTongue, entityGetMidPoint(pPlayer->pBody));
                pPlayer->state = RELEASE;
                break;
        }
    }

    if (getKeyState(pInputs, TOGGLE_GODMODE_INPUT) == KEY_STATE_DOWN) {
        switch (pPlayer->state) {
            case FLYING:
                pPlayer->state = FALLING;
                break;
            default:
                pPlayer->state = FLYING;
        }
    }

    return;
}

void playerUpdateAnimation(Player *pPlayer) {
    int offset = 0;
    if (pPlayer->velocity.x < 0.0f) {
        offset = 32;
    }
    else if (pPlayer->velocity.x == 0.0f) {
        offset = pPlayer->sheetPosition.x;
    }

    switch (pPlayer->state) {
        case IDLE:
        case RUNNING:
        case FLYING:
            pPlayer->sheetPosition.y = 0;
            break;
        case FALLING:
            pPlayer->sheetPosition.y = 64;
            break;
        case JUMPING:
            pPlayer->sheetPosition.y = 32;
            break;
        case SHOOTING:
        case ROTATING:
            if ((2 * M_PI - (M_PI / 12.0f)) < pPlayer->referenceAngle) {
                offset = 64;
                pPlayer->sheetPosition.y = 32;
            }
            else if (pPlayer->referenceAngle < (M_PI / 12.0f)) {
                offset = 64;
                pPlayer->sheetPosition.y = 32;
            }
            else if ((M_PI / 12.0f) < pPlayer->referenceAngle && pPlayer->referenceAngle < ((M_PI / 2.0f) - (M_PI / 12.0f))) {
                offset = 64;
                pPlayer->sheetPosition.y = 64;
            }
            else if (((M_PI / 2.0f) - (M_PI / 12.0f)) < pPlayer->referenceAngle && pPlayer->referenceAngle < ((M_PI / 2.0f) + (M_PI / 12.0f))) {
                offset = 64;
                pPlayer->sheetPosition.y = 96;
            }
            else if (((M_PI / 2.0f) + (M_PI / 12.0f)) < pPlayer->referenceAngle && pPlayer->referenceAngle < (M_PI - (M_PI / 12.0f))) {
                offset = 96;
                pPlayer->sheetPosition.y = 64;
            }
            else if ((M_PI - (M_PI / 12.0f)) < pPlayer->referenceAngle && pPlayer->referenceAngle < (M_PI + (M_PI / 12.0f))) {
                offset = 96;
                pPlayer->sheetPosition.y = 32;
            }
            else if ((M_PI + (M_PI / 12.0f)) < pPlayer->referenceAngle && pPlayer->referenceAngle < (((3 * M_PI) / 2.0f) - (M_PI / 12.0f))) {
                offset = 96;
                pPlayer->sheetPosition.y = 0;
            }
            else if ((((3 * M_PI) / 2.0f) - (M_PI / 12.0f)) < pPlayer->referenceAngle && pPlayer->referenceAngle < (((3 * M_PI) / 2.0f) + (M_PI / 12.0f))) {
                offset = 96;
                pPlayer->sheetPosition.y = 96;
            }
            else if ((((3 * M_PI) / 2.0f) + (M_PI / 12.0f)) < pPlayer->referenceAngle && pPlayer->referenceAngle < (M_PI * 2.0f - (M_PI / 12.0f))) {
                offset = 64;
                pPlayer->sheetPosition.y = 0;
            }
            break;
    }

    pPlayer->sheetPosition.x = offset;
    return;
}

void playerUpdateState(Player *pPlayer) {
    switch (pPlayer->state) {
        case IDLE:
        case RUNNING:
            pPlayer->gravityModifier = 0.0f;
            break;
        case FALLING:
            if (pPlayer->jumpTimer > 0) {
                pPlayer->jumpTimer = 0;
            }
            pPlayer->gravityModifier = 1.0f;
            break;
        case JUMPING:
            pPlayer->gravityModifier = 0.75f;
            if (pPlayer->jumpTimer > 0) {
                pPlayer->jumpTimer--;
                if (pPlayer->jumpTimer == 0) {pPlayer->state = FALLING;}
            }
            break;
        case FLYING:
        case ROTATING:
        case SHOOTING:
        case RELEASE:
        case LOCKED:
            pPlayer->gravityModifier = 0.0f;
            break;
    }

    if (pPlayer->velocity.y >= MAX_GRAVITY_VELOCITY) {
        pPlayer->velocity.y = MAX_GRAVITY_VELOCITY;
    }
    else {
        pPlayer->velocity.y += GRAVITY_ACCELERATION*pPlayer->gravityModifier;
    }

    return;
}

Vec2 rotationCalculations(Player *pPlayer, float deltaTime) {
    Vec2 newPosition;
    pPlayer->referenceAngle += (M_PI/180) * deltaTime * pPlayer->rotateVelocity;
    if (pPlayer->referenceAngle >= 2*M_PI) {
        pPlayer->referenceAngle -= 2*M_PI;
    }
    else if (pPlayer->referenceAngle <= 0.0f) {
        pPlayer->referenceAngle += 2*M_PI;
    }

    newPosition.x =(entityGetMidPoint(pPlayer->pBody).x + cosf(pPlayer->referenceAngle) * pPlayer->radius);
    newPosition.y =(entityGetMidPoint(pPlayer->pBody).y + sinf(pPlayer->referenceAngle) * pPlayer->radius);
    
    return newPosition;
}

Vec2 playerUpdatePosition(Player *pPlayer, float deltaTime) {
    Vec2 returnVector;
    switch(pPlayer->state) {
        case LOCKED:
            break;
        case SHOOTING:
        case RELEASE:
            tongueUpdate(pPlayer->pTongue, entityGetMidPoint(pPlayer->pBody), deltaTime);
            if (tongueGetState(pPlayer->pTongue) == MAX_EXTENSION) { tongueSetVelocity(pPlayer->pTongue, entityGetMidPoint(pPlayer->pBody)); }
            if (tongueGetLength(pPlayer->pTongue) == 0.0f) { pPlayer->state = IDLE; }
            break;
        case ROTATING:
            returnVector = rotationCalculations(pPlayer, deltaTime);
            playerUpdateAnimation(pPlayer);
            break;
        default:
            Vec2 scaledVelocity = pPlayer->velocity;
            vectorScale(&scaledVelocity, deltaTime);
            entityMove(pPlayer->pBody, scaledVelocity);
            tongueSetPosition(pPlayer->pTongue, entityGetMidPoint(pPlayer->pBody));
    }

    return returnVector;
}

int playerCheckCollision(Player *pPlayer, Hitbox *pObject) {
    int collisionDetected = 0;    
    Hitbox *pPlayerHitbox = entityGetHitbox(pPlayer->pBody);
    if (checkCollision(pPlayerHitbox, pObject)) {
        Vec2 correction = rectVsRect(pPlayerHitbox, pObject);
        entityCollisionResponse(pPlayer->pBody, correction);
        collisionDetected = hitboxOrientation(pPlayerHitbox, pObject);
        switch (collisionDetected) {
            case OBJECT_IS_NORTH:
                switch (pPlayer->state) {
                    case RUNNING:
                    case FALLING:
                        pPlayer->state = IDLE;
                        break;
                }
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

void playerSetPosition(Player *pPlayer, Vec2 newPosition) {
    entitySetPosition(pPlayer->pBody, newPosition);
    return;
}

bool playerSetState(Player *pPlayer, int newState) {
    bool stateWasChanged = false;
    switch (newState) {
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
            break;
        case RETRACTING:
            stateWasChanged = true;
            break;
        default:
            break;   
    }

    if (stateWasChanged) { pPlayer->state = newState; }

    return stateWasChanged;
}

bool playerSetRadius(Player *pPlayer, float radius) {
    if (radius <= 0.0f) {
        return false;
    }

    pPlayer->radius = radius;
    return true;
}

bool playerSetReferenceAngle(Player *pPlayer, float newAngle) {
    pPlayer->referenceAngle = newAngle;
    return true;
}

bool playerSetSheetPosition(Player *pPlayer, Vec2 const newPosition) {
    if (newPosition.x < 0.0f || newPosition.y < 0.0f) { return false; }

    pPlayer->sheetPosition.x = newPosition.x;
    pPlayer->sheetPosition.y = newPosition.y;
    return true;
}

int playerGetState(Player const *pPlayer) {
    return pPlayer->state;
}

Entity *playerGetBody(Player const *pPlayer) {
    return pPlayer->pBody;
}

Tongue *playerGetTongue(Player const *pPlayer) {
    return pPlayer->pTongue;
}

Hitbox *playerGetBodyHitbox(Player const *pPlayer) {
    return entityGetHitbox(pPlayer->pBody);
}

SDL_Texture *playerGetBodyTexture(Player const *pPlayer) {
    return entityGetTexture(pPlayer->pBody);
}

Vec2 playerGetPosition(Player const *pPlayer) {
    return entityGetPosition(pPlayer->pBody);
}

Vec2 playerGetVelocity(Player const *pPlayer) {
    return pPlayer->velocity;
}

SDL_Rect playerGetSheetPosition(Player const *pPlayer) {
    return pPlayer->sheetPosition;
}

float playerGetReferenceAngle(Player const *pPlayer) {
    return pPlayer->referenceAngle;
}

float playerGetRotateVelocity(Player const *pPlayer) {
    return pPlayer->rotateVelocity;
}

void playerOverrideState(Player *pPlayer, PlayerState newState) {
    pPlayer->state = newState;
    return;
}

void playerOverrideVelocity(Player *pPlayer, Vec2 newVelocity, float newRotation) {
    pPlayer->velocity = newVelocity;
    pPlayer->rotateVelocity = newRotation;
    return;
}

void destroyPlayer(Player *pPlayer) {
    if (pPlayer == NULL) { return; }

    SDL_DestroyTexture(entityGetTexture(pPlayer->pBody));
    destroyEntity(pPlayer->pBody);
    destroyTongue(pPlayer->pTongue);
    free(pPlayer);
    return;
}
