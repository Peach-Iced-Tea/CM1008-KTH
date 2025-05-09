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

typedef struct {
    bool jump;
    bool shoot;
} ActionLock;

struct player {
    Entity body;
    Tongue *pTongue;
    Vec2 velocity;
    ActionLock lock;
    PlayerState state;
    float rotateVelocity;
    int jumpTimer;
    float radius;
    float referenceAngle;
    float targetAngle;
    Entity *pGrabbedEntity;
};

void initActionLock(ActionLock *pLock) {
    pLock->jump = false;
    pLock->shoot = false;
    return;
}

Player *createPlayer(Vec2 position, int id) {
    if (id < 0 || id > 1) { return NULL; }

    Player *pPlayer = malloc(sizeof(Player));
    if (entityInitData(&(pPlayer->body), position, ENTITY_PLAYER, HITBOX_PLAYER)) { return NULL; }

    pPlayer->pTongue = createTongue(entityGetMidPoint(pPlayer->body));
    if (pPlayer->pTongue == NULL) { return NULL; }

    pPlayer->velocity = createVector(0.0f, 0.0f);
    initActionLock(&(pPlayer->lock));
    pPlayer->state = IDLE;
    pPlayer->rotateVelocity = 0.0f;

    pPlayer->jumpTimer = 0;
    pPlayer->radius = 0.0f;
    pPlayer->referenceAngle = 0.0f;
    pPlayer->targetAngle = 0.0f;

    pPlayer->pGrabbedEntity = NULL;
    return pPlayer;
}

void playerHandleInput(Player *pPlayer, Input const *pInput) {
    Vec2 movement = createVector(0.0f, 0.0f);
    switch (pPlayer->state) {
        case SHOOTING:
        case RELEASE:
        case ROTATING:
            break;
        case FLYING:
            if (getKeyState(pInput, MOVE_UP_INPUT)) { movement.y = -PLAYER_VELOCITY; }
            if (getKeyState(pInput, MOVE_DOWN_INPUT)) { movement.y += PLAYER_VELOCITY; }
        default:
            if (getKeyState(pInput, MOVE_LEFT_INPUT)) { movement.x = -PLAYER_VELOCITY; }
            if (getKeyState(pInput, MOVE_RIGHT_INPUT)) { movement.x += PLAYER_VELOCITY; }
    }
    
    if (getKeyState(pInput, JUMP_INPUT) && !pPlayer->lock.jump) {
        switch (pPlayer->state) {
            case IDLE:
            case RUNNING:
                pPlayer->velocity.y = -JUMP_VELOCITY;
                pPlayer->jumpTimer = JUMP_TIMER;
                pPlayer->lock.jump = true;
                pPlayer->state = JUMPING;
                break;
        }
    }
    else if (getKeyState(pInput, JUMP_INPUT) == KEY_STATE_UP) {
        pPlayer->lock.jump = false;
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

    if (getMouseState(pInput, MOUSE_LEFT) && !pPlayer->lock.shoot) {
        switch (pPlayer->state) {
            case IDLE:
                pPlayer->lock.shoot = true;
                pPlayer->state = SHOOTING;
                pPlayer->referenceAngle = vectorGetAngle(entityGetMidPoint(pPlayer->body), tongueGetInfo(pPlayer->pTongue).mousePosition);
                tongueUpdateVelocity(pPlayer->pTongue, entityGetMidPoint(pPlayer->body));
                break;
        }
    }
    else if (getMouseState(pInput, MOUSE_LEFT) == KEY_STATE_UP) {
        pPlayer->lock.shoot = false;
        switch (pPlayer->state) {
            case SHOOTING:
            case ROTATING:
                tongueUpdateVelocity(pPlayer->pTongue, entityGetMidPoint(pPlayer->body));
                pPlayer->state = RELEASE;
                break;
        }
    }

    if (getKeyState(pInput, TOGGLE_GODMODE_INPUT) == KEY_STATE_DOWN) {
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
    SDL_Rect animation;
    animation.x = 0;
    animation.y = 0;
    animation.w = pPlayer->body.source.w;
    animation.h = pPlayer->body.source.h;
    if (pPlayer->velocity.x < 0.0f) {
        animation.x = 32;
    }
    else if (pPlayer->velocity.x == 0.0f) {
        animation.x = pPlayer->body.source.x;
    }

    switch (pPlayer->state) {
        case IDLE:
        case RUNNING:
        case FLYING:
            animation.y = 0;
            break;
        case FALLING:
            animation.y = 64;
            break;
        case JUMPING:
            animation.y = 32;
            break;
        case SHOOTING:
        case ROTATING:
            if ((2 * M_PI - (M_PI / 12.0f)) < pPlayer->referenceAngle) {
                animation.x = 64;
                animation.y = 32;
            }
            else if (pPlayer->referenceAngle < (M_PI / 12.0f)) {
                animation.x = 64;
                animation.y = 32;
            }
            else if ((M_PI / 12.0f) < pPlayer->referenceAngle && pPlayer->referenceAngle < ((M_PI / 2.0f) - (M_PI / 12.0f))) {
                animation.x = 64;
                animation.y = 64;
            }
            else if (((M_PI / 2.0f) - (M_PI / 12.0f)) < pPlayer->referenceAngle && pPlayer->referenceAngle < ((M_PI / 2.0f) + (M_PI / 12.0f))) {
                animation.x = 64;
                animation.y = 96;
            }
            else if (((M_PI / 2.0f) + (M_PI / 12.0f)) < pPlayer->referenceAngle && pPlayer->referenceAngle < (M_PI - (M_PI / 12.0f))) {
                animation.x = 96;
                animation.y = 64;
            }
            else if ((M_PI - (M_PI / 12.0f)) < pPlayer->referenceAngle && pPlayer->referenceAngle < (M_PI + (M_PI / 12.0f))) {
                animation.x = 96;
                animation.y = 32;
            }
            else if ((M_PI + (M_PI / 12.0f)) < pPlayer->referenceAngle && pPlayer->referenceAngle < (((3 * M_PI) / 2.0f) - (M_PI / 12.0f))) {
                animation.x = 96;
                animation.y = 0;
            }
            else if ((((3 * M_PI) / 2.0f) - (M_PI / 12.0f)) < pPlayer->referenceAngle && pPlayer->referenceAngle < (((3 * M_PI) / 2.0f) + (M_PI / 12.0f))) {
                animation.x = 96;
                animation.y = 96;
            }
            else if ((((3 * M_PI) / 2.0f) + (M_PI / 12.0f)) < pPlayer->referenceAngle && pPlayer->referenceAngle < (M_PI * 2.0f - (M_PI / 12.0f))) {
                animation.x = 64;
                animation.y = 0;
            }
            break;
    }

    entitySetSource(&(pPlayer->body), animation);
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
    pPlayer->referenceAngle = vectorGetAngle(entityGetMidPoint(pPlayer->body), entityGetMidPoint(*(pPlayer->pGrabbedEntity)));
    pPlayer->referenceAngle += (M_PI/180) * deltaTime * pPlayer->rotateVelocity;
    if (pPlayer->referenceAngle >= 2*M_PI) {
        pPlayer->referenceAngle -= 2*M_PI;
    }
    else if (pPlayer->referenceAngle <= 0.0f) {
        pPlayer->referenceAngle += 2*M_PI;
    }

    newPosition.x = (entityGetMidPoint(pPlayer->body).x + cosf(pPlayer->referenceAngle) * pPlayer->radius);
    newPosition.y = (entityGetMidPoint(pPlayer->body).y + sinf(pPlayer->referenceAngle) * pPlayer->radius);

    newPosition.x -= (*pPlayer->pGrabbedEntity).frame.w*0.5f;
    newPosition.y -= (*pPlayer->pGrabbedEntity).frame.h*0.5f;
    
    return newPosition;
}

void playerUpdatePosition(Player *pPlayer, float deltaTime) {
    switch (pPlayer->state) {
        case LOCKED:
            break;
        case SHOOTING:
        case RELEASE:
            if (pPlayer->pGrabbedEntity != NULL) { pPlayer->pGrabbedEntity = NULL; }
            tongueUpdatePosition(pPlayer->pTongue, entityGetMidPoint(pPlayer->body), deltaTime);
            if (tongueGetInfo(pPlayer->pTongue).length == 0.0f) {
                if (pPlayer->body.source.x == 64) {
                    pPlayer->body.source.x = 0;
                }
                else if (pPlayer->body.source.x == 96) {
                    pPlayer->body.source.x = 32;
                }
                pPlayer->state = IDLE;
            }
            break;
        case ROTATING:
            Vec2 newPosition = rotationCalculations(pPlayer, deltaTime);
            playerUpdateAnimation(pPlayer);
            entitySetPosition(pPlayer->pGrabbedEntity, newPosition);
            tongueCalculateShaft(pPlayer->pTongue, entityGetMidPoint(pPlayer->body), entityGetMidPoint(*(pPlayer->pGrabbedEntity)));
            break;
        default:
            Vec2 scaledVelocity = pPlayer->velocity;
            vectorScale(&scaledVelocity, deltaTime);
            entityMove(&(pPlayer->body), scaledVelocity);
            tongueSetPosition(pPlayer->pTongue, entityGetMidPoint(pPlayer->body));
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
    Hitbox *pPlayerHitbox = pPlayer->body.pHitbox;
    if (hitboxCheckCollision(pPlayerHitbox, pObject)) {
        Vec2 correction = rectVsRect(pPlayerHitbox, pObject);
        entityCollisionResponse(&(pPlayer->body), correction);
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
        Hitbox *pGrabbedHitbox = pPlayer->pGrabbedEntity->pHitbox;
        if (hitboxCheckCollision(pGrabbedHitbox, pObject)) {
            Vec2 correction = rectVsRect(pGrabbedHitbox, pObject);
            entityCollisionResponse(pPlayer->pGrabbedEntity, correction);
        }
    }

    return collisionDetected;
}

void playerSetPosition(Player *pPlayer, Vec2 newPosition) {
    entitySetPosition(&(pPlayer->body), newPosition);
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

    pPlayer->body.source.x = newPosition.x;
    pPlayer->body.source.y = newPosition.y;
    return true;
}

void playerSetGrabbedEntity(Player *pPlayer, Entity *pEntity, Player *pTeammate) {
    if (pEntity != NULL && pTeammate != NULL) { return; }

    if (pEntity != NULL) {
        pPlayer->referenceAngle = vectorGetAngle(entityGetMidPoint(pPlayer->body), entityGetMidPoint((*pEntity)));
        pPlayer->radius = vectorLength(entityGetMidPoint(pPlayer->body), entityGetMidPoint((*pEntity)));
        pPlayer->pGrabbedEntity = pEntity;
    }

    if (pTeammate != NULL) {
        pPlayer->referenceAngle = vectorGetAngle(entityGetMidPoint(pPlayer->body), entityGetMidPoint(pTeammate->body));
        pPlayer->radius = vectorLength(entityGetMidPoint(pPlayer->body), entityGetMidPoint(pTeammate->body));
        pPlayer->pGrabbedEntity = &(pTeammate->body);
    }

    return;
}

PlayerInfo playerGetInfo(Player const *pPlayer) {
    PlayerInfo info;
    info.position = entityGetPosition(pPlayer->body);
    info.velocity = pPlayer->velocity;
    info.sheetPosition = pPlayer->body.source;
    info.rotateVelocity = pPlayer->rotateVelocity;
    info.tongueAngle = pPlayer->referenceAngle;
    info.state = pPlayer->state;
    return info;
}

Entity playerGetBody(Player const *pPlayer) {
    return pPlayer->body;
}

Hitbox *playerGetBodyHitbox(Player const *pPlayer) {
    return pPlayer->body.pHitbox;
}

Vec2 playerGetMidPoint(Player const *pPlayer) {
    return entityGetMidPoint(pPlayer->body);
}

Tongue *playerGetTongue(Player const *pPlayer) {
    return pPlayer->pTongue;
}

void playerOverrideMove(Player *pPlayer, Vec2 velocity) {
    entityMove(&(pPlayer->body), velocity);
    return;
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

    destroyHitbox(pPlayer->body.pHitbox);
    destroyTongue(pPlayer->pTongue);
    free(pPlayer);
    return;
}
