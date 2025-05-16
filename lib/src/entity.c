#include "entity.h"

#define ENTITY_DEFAULT_SOURCE_X 0
#define ENTITY_DEFAULT_SOURCE_Y 0
#define ENTITY_DEFAULT_SOURCE_WIDTH 32
#define ENTITY_DEFAULT_SOURCE_HEIGHT 32
#define ENTITY_DEFAULT_FLIP SDL_FLIP_NONE
#define ENTITY_DEFAULT_ANGLE 0.0f

struct hitbox {
    Vec2 position;
    Vec2 halfSize;
};

typedef struct {
    Vec2 min;
    Vec2 max;
} MinMax;

Hitbox *createPlayerHitbox(Vec2 const position, float w, float h) {
    Hitbox *pHitbox = malloc(sizeof(Hitbox));
    w *= 0.25f;
    h *= 0.25f;
    pHitbox->position = createVector(position.x+w*2, position.y+(2*h+h));
    pHitbox->halfSize = createVector(w, h);
    return pHitbox;
}

Hitbox *createFullBlockHitbox(Vec2 const position, float w, float h) {
    Hitbox *pHitbox = malloc(sizeof(Hitbox));
    w *= 0.5f;
    h *= 0.5f;
    pHitbox->position = createVector(position.x+w, position.y+h);
    pHitbox->halfSize = createVector(w, h);
    return pHitbox;
}

Hitbox *createHalfBlockHitboxVertical(Vec2 const position, float w, float h, HitboxType type) {
    Hitbox *pHitbox = malloc(sizeof(Hitbox));
    w *= 0.5f;
    h *= 0.25f;
    pHitbox->halfSize = createVector(w, h);
    switch (type) {
        case HITBOX_HALF_BLOCK_TOP:
            pHitbox->position = createVector(position.x+w, position.y+h);
            break;
        case HITBOX_HALF_BLOCK_BOTTOM:
            pHitbox->position = createVector(position.x+w, position.y+3*h);
            break;
    }

    return pHitbox;
}

Hitbox *createHalfBlockHitboxHorizontal(Vec2 const position, float w, float h, HitboxType type) {
    Hitbox *pHitbox = malloc(sizeof(Hitbox));
    w *= 0.25f;
    h *= 0.5f;
    pHitbox->halfSize = createVector(w, h);
    switch (type) {
        case HITBOX_HALF_BLOCK_LEFT:
            pHitbox->position = createVector(position.x+w, position.y+h);
            break;
        case HITBOX_HALF_BLOCK_RIGHT:
            pHitbox->position = createVector(position.x+3*w, position.y+h);
            break;
    }

    return pHitbox;
}

Hitbox *createEndHitbox(Vec2 const position, float w, float h) {
    Hitbox *pHitbox = malloc(sizeof(Hitbox));
    w *= 3.5f;
    h *= 3.5f;
    pHitbox->position = createVector(position.x+w, position.y+h);
    pHitbox->halfSize = createVector(w, h);
    return pHitbox;
}

Hitbox *createHitbox(Vec2 const position, float w, float h, int hitboxType) {
    Hitbox *pHitbox = NULL;
    switch (hitboxType) {
        case HITBOX_PLAYER:
            pHitbox = createPlayerHitbox(position, w, h);
            break;
        case HITBOX_FULL_BLOCK:
            pHitbox = createFullBlockHitbox(position, w, h);
            break;
        case HITBOX_HALF_BLOCK_TOP:
        case HITBOX_HALF_BLOCK_BOTTOM:
            pHitbox = createHalfBlockHitboxVertical(position, w, h, hitboxType);
            break;
        case HITBOX_HALF_BLOCK_LEFT:
        case HITBOX_HALF_BLOCK_RIGHT:
            pHitbox = createHalfBlockHitboxHorizontal(position, w, h, hitboxType);
            break;
        case HITBOX_END:
            pHitbox = createEndHitbox(position, w, h);
        case HITBOX_NONE:
            break;
    }

    return pHitbox;
}

int entityInitData(Entity *pEntity, Vec2 position, int entityType, int hitboxType) {
    pEntity->frame.x = position.x;
    pEntity->frame.y = position.y;
    pEntity->frame.w = 32.0f;
    pEntity->frame.h = 32.0f;
    switch (entityType) {
        case ENTITY_PLAYER:
            pEntity->frame.w = 48.0f;
            pEntity->frame.h = 48.0f;
            break;
        case ENTITY_TONGUE_TIP:
            pEntity->frame.w = 12.0f;
            pEntity->frame.h = 12.0f;
            break;
        case ENTITY_TONGUE_SHAFT:
            pEntity->frame.w = 0.0f;
            pEntity->frame.h = 20.0f;
            break;
        case ENTITY_CROSSHAIR:
            pEntity->frame.w = 12.0f;
            pEntity->frame.h = 12.0f;
            break;
        case ENTITY_END:
            pEntity->frame.w = 32.0f;
            pEntity->frame.h = 32.0f;
    }

    pEntity->source.x = ENTITY_DEFAULT_SOURCE_X;
    pEntity->source.y = ENTITY_DEFAULT_SOURCE_Y;
    pEntity->source.w = ENTITY_DEFAULT_SOURCE_WIDTH;
    pEntity->source.h = ENTITY_DEFAULT_SOURCE_HEIGHT;
    pEntity->flip = ENTITY_DEFAULT_FLIP;
    pEntity->angle = ENTITY_DEFAULT_ANGLE;

    switch (hitboxType) {
        case HITBOX_NONE:
            pEntity->pHitbox = NULL;
            break;
        default:
            pEntity->pHitbox = createHitbox(position, pEntity->frame.w, pEntity->frame.h, hitboxType);
            if (pEntity->pHitbox == NULL) { return 1; }
    }

    pEntity->type = entityType;
    return 0;
}

void entityCollisionResponse(Entity *pEntity, Vec2 const correction) {
    pEntity->frame.x += correction.x;
    pEntity->frame.y += correction.y;
    hitboxPositionAdd(pEntity->pHitbox, correction);
    return;
}

void entityMove(Entity *pEntity, Vec2 velocity) {
    pEntity->frame.x += velocity.x;
    pEntity->frame.y += velocity.y;
    hitboxPositionAdd(pEntity->pHitbox, velocity);
    return;
}

void entitySetPosition(Entity *pEntity, Vec2 const newPosition) {
    pEntity->frame.x = newPosition.x;
    pEntity->frame.y = newPosition.y;
    destroyHitbox(pEntity->pHitbox);
    pEntity->pHitbox = createHitbox(newPosition, pEntity->frame.w, pEntity->frame.h, HITBOX_PLAYER);
    return;
}

void entitySetSource(Entity *pEntity, SDL_Rect newSource) {
    pEntity->source = newSource;
    return;
}

Vec2 entityGetPosition(Entity const entity) {
    return createVector(entity.frame.x, entity.frame.y);
}

Vec2 entityGetMidPoint(Entity const entity) {
    Vec2 midpoint;
    midpoint.x = entity.frame.x + entity.frame.w * 0.5f;
    midpoint.y = entity.frame.y + entity.frame.h * 0.5f;
    return midpoint;
}

void hitboxPositionAdd(Hitbox *pHitbox, Vec2 const vector) {
    vectorAdd(&pHitbox->position, pHitbox->position, vector);
    return;
}

void hitboxPositionSub(Hitbox *pHitbox, Vec2 const vector) {
    vectorSub(&pHitbox->position, pHitbox->position, vector);
    return;
}

void getMinMax(MinMax *box, Vec2 const position, Vec2 const offset) {
    vectorSub(&box->min, position, offset);
    vectorAdd(&box->max, position, offset);
    return;
}

bool hitboxCheckCollision(Hitbox const *pObject1, Hitbox const *pObject2) {
    bool collisionDetected = true;
    MinMax box1, box2;
    getMinMax(&box1, pObject1->position, pObject1->halfSize);
    getMinMax(&box2, pObject2->position, pObject2->halfSize);

    if (box1.max.x < box2.min.x || box1.min.x > box2.max.x) {
        collisionDetected = false;
    }

    if (collisionDetected) {
        if (box1.max.y < box2.min.y || box1.min.y > box2.max.y) {
            collisionDetected = false;
        }
    }

    return collisionDetected;
}

int hitboxOrientation(Hitbox const *pObject, Hitbox const *pReference) {
    MinMax box1, box2;
    getMinMax(&box1, pObject->position, pObject->halfSize);
    getMinMax(&box2, pReference->position, pReference->halfSize);

    if (box1.max.y <= box2.min.y) { return OBJECT_IS_NORTH; }
    else if (box1.min.y >= box2.max.y) { return OBJECT_IS_SOUTH; }
    else if (box1.max.x <= box2.min.x) { return OBJECT_IS_WEST; }
    else { return OBJECT_IS_EAST; }
}

Vec2 rectVsRect(Hitbox const *pObject, Hitbox const *pReference) {
    float scaler1 = pObject->halfSize.y/pObject->halfSize.x;
    float scaler2 = pReference->halfSize.y/pReference->halfSize.x;
    float x_diff = pObject->position.x-pReference->position.x;
    float y_diff = pObject->position.y-pReference->position.y;

    MinMax box1, box2;
    getMinMax(&box1, pObject->position, pObject->halfSize);
    getMinMax(&box2, pReference->position, pReference->halfSize);

    Vec2 correction;
    if (abs(x_diff*scaler1*scaler2) > abs(y_diff)) {
        if (x_diff > 0.0f) {
            correction = createVector(box2.max.x-box1.min.x, 0.0f);
        }
        else {
            correction = createVector(box2.min.x-box1.max.x, 0.0f);
        }
    }
    else if (abs(x_diff*scaler1*scaler2) < abs(y_diff)) {
        if (y_diff > 0.0f) {
            correction = createVector(0.0f, box2.max.y-box1.min.y);
        }
        else {
            correction = createVector(0.0f, box2.min.y-box1.max.y);
        }
    }
    else {
        correction = createVector(0.0f, 0.0f);
    }

    return correction;
}

float rayVsRect(Hitbox const *pObject, Hitbox const *pReference) {
    // Add logic for collision prediction along an objects trajectory.
}

bool pointVsRect(Hitbox const *pHitbox, Vec2 const point) {
    bool collisionDetected = true;
    MinMax box;
    getMinMax(&box, pHitbox->position, pHitbox->halfSize);

    if (box.max.x < point.x || box.min.x > point.x) {
        collisionDetected = false;
    }

    if (collisionDetected) {
        if (box.max.y < point.y || box.min.y > point.y) {
            collisionDetected = false;
        }
    }

    return collisionDetected;
}

Vec2 hitboxGetPosition(Hitbox const *pHitbox) {
    return pHitbox->position;
}

Vec2 hitboxGetHalfSize(Hitbox const *pHitbox) {
    return pHitbox->halfSize;
}

void destroyHitbox(Hitbox *pHitbox) {
    if (pHitbox == NULL) { return; }

    free(pHitbox);
    return;
}