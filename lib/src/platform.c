#include "platform.h"

#define PLATFORM_SPEED 2.0f
#define MAX_PLATFORM_WIDTH 7
#define MAX_PLATFORM_SIZE MAX_PLATFORM_WIDTH+(MAX_PLATFORM_WIDTH-2)

typedef enum PlatformDirection {
    PLATFORM_HORIZONTAL, PLATFORM_VERTICAL
} PlatformDirection;

struct movablePlatform {
    Entity body[MAX_PLATFORM_SIZE];
    Vec2 velocity;
    Vec2 relativePosition;
    int size;
    PlatformDirection direction;
    PlatformType type;
};

MovablePlatform *createPlatform(Vec2 position, int width, PlatformType type) {
    if (width > MAX_PLATFORM_WIDTH) { 
        printf("Error: Given width %d exceeds maximum width of %d\n", width, MAX_PLATFORM_WIDTH);
        return NULL;
    }

    MovablePlatform *pPlatform = malloc(sizeof(MovablePlatform));
    Vec2 initPosition = createVector(position.x, position.y);
    pPlatform->size = width;
    switch (type) {
        case PLATFORM_FLAT:
            for (int i = 0; i < pPlatform->size; i++) {
                if (entityInitData(&(pPlatform->body[i]), initPosition, ENTITY_DEFAULT, HITBOX_FULL_BLOCK)) { return NULL; }

                if (i > 0 && i < pPlatform->size-1) {
                    pPlatform->body[i].source.x = 32;
                }
                else if (i == pPlatform->size-1) {
                    pPlatform->body[i].source.x = 64;
                }
                initPosition.x += 32.0f;
            }
            break;
        case PLATFORM_CONVEX:
            if (pPlatform->size+(width-2) > MAX_PLATFORM_SIZE) { return NULL; }

            for (int i = 0; i < pPlatform->size; i++) {
                if (entityInitData(&(pPlatform->body[i]), initPosition, ENTITY_DEFAULT, HITBOX_FULL_BLOCK)) { return NULL; }

                if (i > 0 && i < pPlatform->size-1) {
                    pPlatform->body[i].source.x = 32;
                }

                initPosition.x += 32.0f;

                if (i == pPlatform->size-1) {
                    pPlatform->body[i].source.x = 64;
                    initPosition.y += 32.0f;
                    initPosition.x = position.x+32.0f;
                    for (int j = 0; j < pPlatform->size-2; j++) {
                        if (entityInitData(&(pPlatform->body[i+j+1]), initPosition, ENTITY_DEFAULT, HITBOX_FULL_BLOCK)) { return NULL; }

                        if (j > 0 && j < pPlatform->size-3) {
                            pPlatform->body[i+j+1].source.x = 32;
                        }
                        else if (j == pPlatform->size-3) {
                            pPlatform->body[i+j+1].source.x = 64;
                        }
                        initPosition.x += 32.0f;
                    }
                }
            }
            pPlatform->size += width-2;
            break;
        default:
            printf("Error: Unknown PlatformType %d", type);
            return NULL;
    }

    pPlatform->velocity.x = PLATFORM_SPEED;
    pPlatform->velocity.y = 0.0f;
    pPlatform->relativePosition = createVector(0.0f, 0.0f);
    pPlatform->direction = PLATFORM_HORIZONTAL;
    pPlatform->type = type;

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