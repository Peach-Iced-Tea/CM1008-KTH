#include "entity.h"

struct entity {
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    Hitbox *pHitbox;
    SDL_Rect currentFrame;
    SDL_Texture *pTexture;
};

Entity *createEntity(Vec2 position, SDL_Texture *pTexture, int hitboxType) {
    Entity *pEntity = malloc(sizeof(Entity));

    pEntity->position = position;
    pEntity->velocity = createVector(0.0f, 0.0f);
    pEntity->acceleration = createVector(0.0f, 0.0f);

    pEntity->pTexture = pTexture;

    pEntity->currentFrame.x = 0;//pPos->x;
    pEntity->currentFrame.y = 0;//pPos->y;
    pEntity->currentFrame.w = 32;
    pEntity->currentFrame.h = 32;

    switch(hitboxType) {
        case HITBOX_PLAYER:
            pEntity->pHitbox = createPlayerHitbox(position, (float)pEntity->currentFrame.w, (float)pEntity->currentFrame.h);
            break;
        case HITBOX_FULL_BLOCK:
            pEntity->pHitbox = createFullBlockHitbox(position, (float)pEntity->currentFrame.w, (float)pEntity->currentFrame.h);
            break;
        case HITBOX_HALF_BLOCK:
            pEntity->pHitbox = createHalfBlockHitbox(position, (float)pEntity->currentFrame.w, (float)pEntity->currentFrame.h);
            break;
    }

    return pEntity;
}

SDL_Texture *getTexture(Entity const *pEntity) {
    return pEntity->pTexture;
}

SDL_Rect getCurrentFrame(Entity const *pEntity) {
    return pEntity->currentFrame;
}

void collisionResponse(Entity *pEntity, Vec2 const correction) {
    vectorAdd(&pEntity->position, pEntity->position, correction);
    hitboxPositionAdd(pEntity->pHitbox, correction);
    return;
}

bool setVelocityX(Entity *pEntity, float v_x) {
    pEntity->velocity.x = v_x;
    return true;
}

bool setVelocityY(Entity *pEntity, float v_y) {
    pEntity->velocity.y = v_y;
    return true;
}

bool setAccelerationX(Entity *pEntity, float a_x) {
    pEntity->acceleration.x = a_x;
    return true;
}

bool setAccelerationY(Entity *pEntity, float a_y) {
    pEntity->acceleration.y = a_y;
    return true;
}

void updatePosition(Entity *pEntity, float const deltaTime) {
    Vec2 velocity = createVector(pEntity->velocity.x, pEntity->velocity.y);
    vectorScale(&velocity, deltaTime);

    // Update the current position of both the Sprite position and the Hitbox position.
    vectorAdd(&pEntity->position, pEntity->position, velocity);
    hitboxPositionAdd(pEntity->pHitbox, velocity);

    return;
}

void updateVelocity(Entity *pEntity, float const timestep) {
    Vec2 acceleration = createVector(pEntity->acceleration.x, pEntity->acceleration.y);
    if (pEntity->velocity.x >= MAX_PLAYER_VELOCITY) {
        pEntity->velocity.x = MAX_PLAYER_VELOCITY;
        acceleration.x = 0.0f;
    }
    else if (pEntity->velocity.x <= -MAX_PLAYER_VELOCITY) {
        pEntity->velocity.x = -MAX_PLAYER_VELOCITY;
        acceleration.x = 0.0f;
    }

    if (pEntity->velocity.y >= MAX_GRAVITY_VELOCITY) {
        pEntity->velocity.y = MAX_GRAVITY_VELOCITY;
        acceleration.y = 0.0f;
    }

    if (acceleration.x != 0.0f || acceleration.y != 0.0f) {
        vectorScale(&acceleration, timestep);
        vectorAdd(&pEntity->velocity, pEntity->velocity, acceleration);
    }

    return;
}

Vec2 getPosition(Entity const *pEntity) {
    return pEntity->position;
}

Vec2 getVelocity(Entity const *pEntity) {
    return pEntity->velocity;
}

Vec2 getAcceleration(Entity const *pEntity) {
    return pEntity->acceleration;
}

Hitbox *getHitbox(Entity const *pEntity) {
    return pEntity->pHitbox;
}

void destroyEntity(Entity *pEntity) {
    if (pEntity == NULL) { return; }

    destroyHitbox(pEntity->pHitbox);
    free(pEntity);
}