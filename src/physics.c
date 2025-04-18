#include "physics.h"

struct hitbox {
    Vec2 position;
    Vec2 halfSize;
};

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

Vec2 getHitboxPosition(Hitbox const *pHitbox) {
    return pHitbox->position;
}

Vec2 getHitboxHalfSize(Hitbox const *pHitbox) {
    return pHitbox->halfSize;
}

bool checkCollision(Hitbox const *pObject1, Hitbox const *pObject2) {
    bool collisionDetected = true;
    Vec2 min1;  // Used to get the top and left edges of the hitbox of pObject.
    Vec2 max1;  // Used to get the bottom and right edges of the hitbox of pObject.
    vectorSub(&min1, pObject1->position, pObject1->halfSize);
    vectorAdd(&max1, pObject1->position, pObject1->halfSize);

    Vec2 min2;  // Used to get the top and left edges of the hitbox of pReference.
    Vec2 max2;  // Used to get the bottom and right edges of the hitbox of pReference.
    vectorSub(&min2, pObject2->position, pObject2->halfSize);
    vectorAdd(&max2, pObject2->position, pObject2->halfSize);

    if (max1.x < min2.x || min1.x > max2.x) {
        collisionDetected = false;
    }

    if (collisionDetected) {
        if (max1.y < min2.y || min1.y > max2.y) {
            collisionDetected = false;
        }
    }

    return collisionDetected;
}

int hitboxOrientation(Hitbox const *pObject, Hitbox const *pReference) {
    Vec2 min1;  // Used to get the top and left edges of the hitbox of pObject.
    Vec2 max1;  // Used to get the bottom and right edges of the hitbox of pObject.
    vectorSub(&min1, pObject->position, pObject->halfSize);
    vectorAdd(&max1, pObject->position, pObject->halfSize);

    Vec2 min2;  // Used to get the top and left edges of the hitbox of pReference.
    Vec2 max2;  // Used to get the bottom and right edges of the hitbox of pReference.
    vectorSub(&min2, pReference->position, pReference->halfSize);
    vectorAdd(&max2, pReference->position, pReference->halfSize);
    if (max1.y <= min2.y) { return OBJECT_IS_NORTH; }
    else if (min1.y >= max2.y) { return OBJECT_IS_SOUTH; }
    else if (max1.x <= min2.x) { return OBJECT_IS_WEST; }
    else { return OBJECT_IS_EAST; }
}

Vec2 rectVsRect(Hitbox const *pObject, Hitbox const *pReference) {
    float scaler1 = pObject->halfSize.y/pObject->halfSize.x;
    float scaler2 = pReference->halfSize.y/pReference->halfSize.x;
    float x_diff = pObject->position.x-pReference->position.x;
    float y_diff = pObject->position.y-pReference->position.y;

    Vec2 min1;  // Used to get the top and left edges of the hitbox of pObject.
    Vec2 max1;  // Used to get the bottom and right edges of the hitbox of pObject.
    vectorSub(&min1, pObject->position, pObject->halfSize);
    vectorAdd(&max1, pObject->position, pObject->halfSize);

    Vec2 min2;  // Used to get the top and left edges of the hitbox of pReference.
    Vec2 max2;  // Used to get the bottom and right edges of the hitbox of pReference.
    vectorSub(&min2, pReference->position, pReference->halfSize);
    vectorAdd(&max2, pReference->position, pReference->halfSize);

    Vec2 correction;
    if (abs(x_diff*scaler1*scaler2) > abs(y_diff)) {
        if (x_diff > 0.0f) {
            correction = createVector(max2.x-min1.x, 0.0f);
        }
        else {
            correction = createVector(min2.x-max1.x, 0.0f);
        }
    }
    else if (abs(x_diff*scaler1*scaler2) < abs(y_diff)) {
        if (y_diff > 0.0f) {
            correction = createVector(0.0f, max2.y-min1.y);
        }
        else {
            correction = createVector(0.0f, min2.y-max1.y);
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

bool touching(Hitbox const *pHitbox, Vec2 const point) {
    bool collisionDetected = true;
    Vec2 min1;  
    Vec2 max1;  
    vectorSub(&min1, pHitbox->position, pHitbox->halfSize);
    vectorAdd(&max1, pHitbox->position, pHitbox->halfSize);

    if (max1.x < point.x || min1.x > point.x) {
        collisionDetected = false;
    }

    if (collisionDetected) {
        if (max1.y < point.y || min1.y > point.y) {
            collisionDetected = false;
        }
    }

    return collisionDetected;
}

void destroyHitbox(Hitbox *pHitbox) {
    if (pHitbox == NULL) { return; }

    free(pHitbox);
}