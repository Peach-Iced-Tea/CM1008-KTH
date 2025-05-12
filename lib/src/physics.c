#include "physics.h"

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

Hitbox *createHalfBlockHitbox(Vec2 const position, float w, float h) {
    Hitbox *pHitbox = malloc(sizeof(Hitbox));
    w *= 0.5f;
    h *= 0.25f;
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
        case HITBOX_HALF_BLOCK:
            pHitbox = createHalfBlockHitbox(position, w, h);
            break;
        case HITBOX_NONE:
            break;
    }

    return pHitbox;
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

bool checkCollision(Hitbox const *pObject1, Hitbox const *pObject2) {
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

Vec2 getHitboxPosition(Hitbox const *pHitbox) {
    return pHitbox->position;
}

Vec2 getHitboxHalfSize(Hitbox const *pHitbox) {
    return pHitbox->halfSize;
}

void destroyHitbox(Hitbox *pHitbox) {
    if (pHitbox == NULL) { return; }

    free(pHitbox);
    return;
}