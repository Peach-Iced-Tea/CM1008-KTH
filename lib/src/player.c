#include "player.h"

#define JUMP_TIMER 15
#define PLAYER_VELOCITY 10.0f   // Initial velocity to be applied on a player upon moving.
#define PLAYER_ACCELERATION 25.0f  // A constant for the acceleration of a player.
#define MAX_PLAYER_VELOCITY 360.0f  // The absolute max value of the velocity a player can reach.
#define JUMP_VELOCITY 780.0f    // The velocity to be applied when a player presses jump.

#define MOVE_UP_INPUT KEY_W
#define MOVE_DOWN_INPUT KEY_S
#define MOVE_LEFT_INPUT KEY_A
#define MOVE_RIGHT_INPUT KEY_D
#define JUMP_INPUT KEY_SPACE
#define TOGGLE_GODMODE_INPUT KEY_G

struct player {
    Entity *pBody;
    Tongue *pTongue;
    PlayerState state;
    Vec2 velocity;
    float rotateVelocity;
    int jumpTimer;
    float radius;
    float referenceAngle;
    float targetAngle;
    SDL_Rect sheetPosition;
    Entity *pGrabbedEntity;
};

Player *createPlayer(Vec2 position, SDL_Renderer *pRenderer, int id) {
    if (id < 0 || id > 1) { return NULL; }

    Player *pPlayer = malloc(sizeof(Player));
    pPlayer->pBody = createEntity(position, NULL, ENTITY_PLAYER, HITBOX_PLAYER);
    pPlayer->pTongue = createTongue(entityGetMidPoint(pPlayer->pBody), pRenderer);

    pPlayer->state = IDLE;
    pPlayer->velocity = createVector(0.0f, 0.0f);
    pPlayer->rotateVelocity = 0.0f;

    pPlayer->jumpTimer = 0;
    pPlayer->radius = 0.0f;
    pPlayer->referenceAngle = 0.0f;
    pPlayer->targetAngle = 0.0f;

    pPlayer->sheetPosition.x = 0;
    pPlayer->sheetPosition.y = 0;
    pPlayer->sheetPosition.w = 32;
    pPlayer->sheetPosition.h = 32;

    pPlayer->pGrabbedEntity = NULL;

    return pPlayer;
}

void playerHandleInput(Player *pPlayer, Input const *pInputs) {
    Vec2 movement = createVector(0.0f, 0.0f);
    switch (pPlayer->state) {
        case SHOOTING:
        case RELEASE:
        case ROTATING:
            break;
        case FLYING:
            if (getKeyState(pInputs, MOVE_UP_INPUT)) { movement.y = -PLAYER_VELOCITY; }
            if (getKeyState(pInputs, MOVE_DOWN_INPUT)) { movement.y += PLAYER_VELOCITY; }
        default:
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

    if ((movement.x > 0.0f && pPlayer->velocity.x <= 0.0f) || (movement.x < 0.0f && pPlayer->velocity.x >= 0.0f) || (movement.x == 0.0f)) {
        pPlayer->velocity.x = movement.x;
    }
    switch (pPlayer->state) {
        case FALLING:
        case JUMPING:
            break;
        default:
            pPlayer->velocity.y = movement.y;
            break;
    }

    if (getMouseState(pInputs, MOUSE_LEFT)) {
        switch (pPlayer->state) {
            case IDLE:
                pPlayer->state = SHOOTING;
                pPlayer->referenceAngle = vectorGetAngle(entityGetMidPoint(pPlayer->pBody), tongueGetInfo(pPlayer->pTongue).mousePosition);
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
                printf("release: state = %d\n", pPlayer->state);
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
    float gravityModifier = 0.0f;
    switch (pPlayer->state) {
        case FALLING:
            gravityModifier = 1.0f;
            if (pPlayer->jumpTimer > 0) {
                pPlayer->jumpTimer = 0;
                pPlayer->velocity.y = -GRAVITY_ACCELERATION;
            }
            break;
        case JUMPING:
            gravityModifier = 0.75f;
            if (pPlayer->jumpTimer > 0) {
                pPlayer->jumpTimer--;
                if (pPlayer->jumpTimer == 0) {pPlayer->state = FALLING;}
            }
            break;
    }

    if (pPlayer->velocity.x > 0.0f) {
        if (pPlayer->velocity.x >= MAX_PLAYER_VELOCITY) {
            pPlayer->velocity.x = MAX_PLAYER_VELOCITY;
        }
        else {
            pPlayer->velocity.x += PLAYER_ACCELERATION;
        }
    }
    else if (pPlayer->velocity.x < 0.0f) {
        if (pPlayer->velocity.x <= -MAX_PLAYER_VELOCITY) {
            pPlayer->velocity.x = -MAX_PLAYER_VELOCITY;
        }
        else {
            pPlayer->velocity.x -= PLAYER_ACCELERATION;
        }
    }

    if (pPlayer->velocity.y >= MAX_GRAVITY_VELOCITY) {
        pPlayer->velocity.y = MAX_GRAVITY_VELOCITY;
    }
    else {
        pPlayer->velocity.y += GRAVITY_ACCELERATION*gravityModifier;
    }

    return;
}

Vec2 rotationCalculations(Player *pPlayer, float deltaTime) {
    Vec2 newPosition;
    pPlayer->referenceAngle = vectorGetAngle(entityGetMidPoint(pPlayer->pBody), entityGetMidPoint(pPlayer->pGrabbedEntity));
    pPlayer->referenceAngle += (M_PI/180) * deltaTime * pPlayer->rotateVelocity;
    if (pPlayer->referenceAngle >= 2*M_PI) {
        pPlayer->referenceAngle -= 2*M_PI;
    }
    else if (pPlayer->referenceAngle <= 0.0f) {
        pPlayer->referenceAngle += 2*M_PI;
    }

    newPosition.x = (entityGetMidPoint(pPlayer->pBody).x + cosf(pPlayer->referenceAngle) * pPlayer->radius);
    newPosition.y = (entityGetMidPoint(pPlayer->pBody).y + sinf(pPlayer->referenceAngle) * pPlayer->radius);

    newPosition.x -= entityGetCurrentFrame(pPlayer->pGrabbedEntity).w*0.5f;
    newPosition.y -= entityGetCurrentFrame(pPlayer->pGrabbedEntity).h*0.5f;
    
    return newPosition;
}

void playerUpdatePosition(Player *pPlayer, float deltaTime) {
    switch (pPlayer->state) {
        case LOCKED:
            break;
        case SHOOTING:
        case RELEASE:
            if (pPlayer->pGrabbedEntity != NULL) { pPlayer->pGrabbedEntity = NULL; }
            tongueUpdate(pPlayer->pTongue, entityGetMidPoint(pPlayer->pBody), deltaTime);
            if (tongueGetInfo(pPlayer->pTongue).length == 0.0f) {
                if (pPlayer->sheetPosition.x == 64) {
                    pPlayer->sheetPosition.x = 0;
                }
                else if (pPlayer->sheetPosition.x == 96) {
                    pPlayer->sheetPosition.x = 32;
                }
                pPlayer->state = IDLE;
            }
            break;
        case ROTATING:
            Vec2 newPosition = rotationCalculations(pPlayer, deltaTime);
            playerUpdateAnimation(pPlayer);
            entitySetPosition(pPlayer->pGrabbedEntity, newPosition);
            tongueCalculateShaft(pPlayer->pTongue, entityGetMidPoint(pPlayer->pBody), entityGetMidPoint(pPlayer->pGrabbedEntity));
            break;
        default:
            Vec2 scaledVelocity = pPlayer->velocity;
            vectorScale(&scaledVelocity, deltaTime);
            entityMove(pPlayer->pBody, scaledVelocity);
            tongueSetPosition(pPlayer->pTongue, entityGetMidPoint(pPlayer->pBody));
    }

    return;
}

void playerCalculateRotation(Player *pPlayer, float targetAngle) {
    if (pPlayer->state != ROTATING) { return; }

    if (pPlayer->referenceAngle == targetAngle) {
        pPlayer->rotateVelocity = 0.0f;
        return;
    }

    float rotateLeft = 0.0f, rotateRight = 0.0f;
    if (targetAngle < pPlayer->referenceAngle) {
        float rotateLeft = pPlayer->referenceAngle - targetAngle;
        float rotateRight = (targetAngle+2*M_PI) - pPlayer->referenceAngle;
        if (rotateLeft <= rotateRight) {
            pPlayer->rotateVelocity = -rotateLeft * (180.0f/M_PI);
        }
        else {
            pPlayer->rotateVelocity = rotateRight * (180.0f/M_PI);
        }
    }
    else {
        float rotateLeft = (pPlayer->referenceAngle+2*M_PI) - targetAngle;
        float rotateRight = targetAngle - pPlayer->referenceAngle;
        if (rotateLeft <= rotateRight) {
            pPlayer->rotateVelocity = -rotateLeft * (180.0f/M_PI);
        }
        else {
            pPlayer->rotateVelocity = rotateRight * (180.0f/M_PI);
        }
    }

    pPlayer->rotateVelocity *= 2.0f;
    return;
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

    if (pPlayer->pGrabbedEntity != NULL) {
        Hitbox *pGrabbedHitbox = entityGetHitbox(pPlayer->pGrabbedEntity);
        if (checkCollision(pGrabbedHitbox, pObject)) {
            Vec2 correction = rectVsRect(pGrabbedHitbox, pObject);
            entityCollisionResponse(pPlayer->pGrabbedEntity, correction);
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
                case LOCKED:
                    break;
                default:
                    stateWasChanged = true;
                    break;
            }
            break;
        case ROTATING:
            switch (pPlayer->state) {
                case LOCKED:
                    break;
                default:
                    stateWasChanged = true;
            }
            break;
        default:
            break;   
    }

    if (stateWasChanged) { pPlayer->state = newState; }

    return stateWasChanged;
}

bool playerSetSheetPosition(Player *pPlayer, Vec2 const newPosition) {
    if (newPosition.x < 0.0f || newPosition.y < 0.0f) { return false; }

    pPlayer->sheetPosition.x = newPosition.x;
    pPlayer->sheetPosition.y = newPosition.y;
    return true;
}

void playerSetGrabbedEntity(Player *pPlayer, Entity *pEntity) {
    if (pEntity == NULL) { return; }

    pPlayer->referenceAngle = vectorGetAngle(entityGetMidPoint(pPlayer->pBody), entityGetMidPoint(pEntity));
    pPlayer->radius = vectorLength(entityGetMidPoint(pPlayer->pBody), entityGetMidPoint(pEntity));
    pPlayer->pGrabbedEntity = pEntity;
    return;
}

PlayerInfo playerGetInfo(Player const *pPlayer) {
    PlayerInfo info;
    info.position = entityGetPosition(pPlayer->pBody);
    info.velocity = pPlayer->velocity;
    info.sheetPosition = pPlayer->sheetPosition;
    info.rotateVelocity = pPlayer->rotateVelocity;
    info.tongueAngle = pPlayer->referenceAngle;
    info.state = pPlayer->state;
    return info;
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

void playerOverrideState(Player *pPlayer, PlayerState newState) {
    pPlayer->state = newState;
    switch (newState) {
        case LOCKED:
            if (pPlayer->jumpTimer > 0) {
                pPlayer->jumpTimer = 0;
            }
            break;
    }
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
