#include "entity.h"

struct entity {
    SDL_FRect currentFrame;
    Vec2 velocity;
    Vec2 acceleration;
    Hitbox *pHitbox;
    SDL_Texture *pTexture;
};

Entity *createEntity(Vec2 position, SDL_Texture *pTexture, int hitboxType) {
    Entity *pEntity = malloc(sizeof(Entity));

    pEntity->currentFrame.x = position.x;
    pEntity->currentFrame.y = position.y;
    pEntity->currentFrame.w = 32.0f;
    pEntity->currentFrame.h = 32.0f;

    pEntity->velocity = createVector(0.0f, 0.0f);
    pEntity->acceleration = createVector(0.0f, 0.0f);

    pEntity->pHitbox = createHitbox(position, pEntity->currentFrame.w, pEntity->currentFrame.h, hitboxType);
    pEntity->pTexture = pTexture;

    return pEntity;
}

SDL_Texture *getTexture(Entity const *pEntity) {
    return pEntity->pTexture;
}

SDL_FRect getCurrentFrame(Entity const *pEntity) {
    return pEntity->currentFrame;
}

void collisionResponse(Entity *pEntity, Vec2 const correction) {
    pEntity->currentFrame.x += correction.x;
    pEntity->currentFrame.y += correction.y;
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
    pEntity->currentFrame.x += velocity.x;
    pEntity->currentFrame.y += velocity.y;
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
    return createVector(pEntity->currentFrame.x, pEntity->currentFrame.y);
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