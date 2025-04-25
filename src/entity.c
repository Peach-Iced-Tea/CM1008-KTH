#include "entity.h"

struct entity {
    SDL_FRect currentFrame;
    Hitbox *pHitbox;
    SDL_Texture *pTexture;
};

Entity *createEntity(Vec2 position, SDL_Texture *pTexture, int entityType, int hitboxType) {
    Entity *pEntity = malloc(sizeof(Entity));

    pEntity->currentFrame.x = position.x;
    pEntity->currentFrame.y = position.y;

    switch (entityType) {
        case ENTITY_PLAYER:
            pEntity->currentFrame.w = 48.0f;
            pEntity->currentFrame.h = 48.0f;
            break;
        default:
            pEntity->currentFrame.w = 32.0f;
            pEntity->currentFrame.h = 32.0f;
            break;
    }

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

void setPosition(Entity *pEntity, Vec2 const vector) {
    pEntity->currentFrame.x = vector.x;
    pEntity->currentFrame.y = vector.y;
    hitboxPositionAdd(pEntity->pHitbox, vector);
    return;
}

void entityMove(Entity *pEntity, Vec2 velocity) {
    pEntity->currentFrame.x += velocity.x;
    pEntity->currentFrame.y += velocity.y;
    hitboxPositionAdd(pEntity->pHitbox, velocity);
    return;
}

Vec2 getPosition(Entity const *pEntity) {
    return createVector(pEntity->currentFrame.x, pEntity->currentFrame.y);
}

Vec2 getMidPoint(Entity const *pEntity) {
    Vec2 midpoint;
    midpoint.x = pEntity->currentFrame.x + pEntity->currentFrame.w * 0.5f;
    midpoint.y = pEntity->currentFrame.y + pEntity->currentFrame.h * 0.5f;
    return midpoint;
}

Hitbox *getHitbox(Entity const *pEntity) {
    return pEntity->pHitbox;
}

void destroyEntity(Entity *pEntity) {
    if (pEntity == NULL) { return; }

    destroyHitbox(pEntity->pHitbox);
    free(pEntity);
    return;
}