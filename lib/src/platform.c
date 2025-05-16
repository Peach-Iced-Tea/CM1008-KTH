#include "platform.h"

struct movablePlatform {
    Entity body[MAX_MOVE_PLATFORMS];
    Vec2 velocity;
    Vec2 relativePosition;
    int size;
    PlatformDirection direction;
};

MovablePlatform *createPlatform(Vec2 position) {
    MovablePlatform *pPlatform = malloc(sizeof(MovablePlatform));
    Vec2 initPosition = createVector(position.x, position.y);
    pPlatform->size = MAX_MOVE_PLATFORMS;
    for (int i = 0; i < pPlatform->size; i++) {
        if (i == 5) {
            initPosition.x = position.x+32.0f;
            initPosition.y += 32.0f;
        }

        if (entityInitData(&(pPlatform->body[i]), initPosition, ENTITY_DEFAULT, HITBOX_FULL_BLOCK)) { return NULL; }

        initPosition.x += 32.0f;
    }

    pPlatform->velocity.x = PLATFORM_SPEED;
    pPlatform->velocity.y = 0.0f;
    pPlatform->relativePosition = createVector(0.0f, 0.0f);
    pPlatform->direction = PLATFORM_RIGHT;

    return pPlatform;
}

void platformMove(MovablePlatform *pPlatform) {
    if (pPlatform->relativePosition.x >= 320.0f) {
        pPlatform->velocity.x = -PLATFORM_SPEED;
    }
    else if (pPlatform->relativePosition.x <= 0.0f) {
        pPlatform->velocity.x = PLATFORM_SPEED;
    }

    for (int i = 0; i < pPlatform->size; i++) {
        entityMove(&(pPlatform->body[i]), pPlatform->velocity);
    }

    vectorAdd(&(pPlatform->relativePosition), pPlatform->relativePosition, pPlatform->velocity);
    return;
}

Entity platformGetBody(MovablePlatform const *pPlatform, int index) {
    return pPlatform->body[index];
}

Hitbox *platformGetHitbox(MovablePlatform const *pPlatform, int index) {
    return pPlatform->body[index].pHitbox;
}

Vec2 platformGetVelocity(MovablePlatform const *pPlatform) {
    return pPlatform->velocity;
}

int platformGetSize(MovablePlatform const *pPlatform) {
    return pPlatform->size;
}

void destroyPlatform(MovablePlatform *pPlatform) {
    if (pPlatform == NULL) { return; }

    free(pPlatform);
    return;
}