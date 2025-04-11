#include "physics.h"

struct hitbox {
    Vec2 position;
    Vec2 halfSize;
};

Hitbox *createHitbox(float p_x, float p_y, float w, float h) {
    Hitbox *pHitbox = malloc(sizeof(Hitbox));
    pHitbox->position = createVector(p_x, p_y);
    pHitbox->halfSize = createVector(w, h);
    return pHitbox;
}

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
    Vec2 min1;  // Used to get the top and left edges of the hitbox of Object1.
    Vec2 max1;  // Used to get the bottom and right edges of the hitbox of Object1.
    Vec2 min2;  // Used to get the top and left edges of the hitbox of Object2.
    Vec2 max2;  // Used to get the bottom and right edges of the hitbox of Object2.

    // Calculate the outer perimeter of the hitbox of Object1.
    vectorSub(&min1, pObject1->position, pObject1->halfSize);
    vectorAdd(&max1, pObject1->position, pObject1->halfSize);

    // Calculate the outer perimeter of the hitbox of Object2.
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

bool hitboxIsAbove(Hitbox const *pObject1, Hitbox const *pObject2) {
    bool isAbove = false;
    Vec2 max1;
    Vec2 min2;
    vectorAdd(&max1, pObject1->position, pObject1->halfSize);
    vectorSub(&min2, pObject2->position, pObject2->halfSize);
    if (max1.y <= min2.y) { isAbove = true; }

    return isAbove;
}

Vec2 rectVsRect(Hitbox const *pObject1, Hitbox const *pObject2) {
    Vec2 min1;  // Used to get the top and left edges of the hitbox of Object1.
    Vec2 max1;  // Used to get the bottom and right edges of the hitbox of Object1.
    Vec2 min2;  // Used to get the top and left edges of the hitbox of Object2.
    Vec2 max2;  // Used to get the bottom and right edges of the hitbox of Object2.
    float scaler1 = pObject1->halfSize.y/pObject1->halfSize.x;
    float scaler2 = pObject2->halfSize.y/pObject2->halfSize.x;
    float x_diff = pObject1->position.x-pObject2->position.x;
    float y_diff = pObject1->position.y-pObject2->position.y;

    // Calculate the outer perimeter of the hitbox of Object1.
    vectorSub(&min1, pObject1->position, pObject1->halfSize);
    vectorAdd(&max1, pObject1->position, pObject1->halfSize);

    // Calculate the outer perimeter of the hitbox of Object2.
    vectorSub(&min2, pObject2->position, pObject2->halfSize);
    vectorAdd(&max2, pObject2->position, pObject2->halfSize);

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

float rayVsRect(Hitbox const *pObject1, Hitbox const *pObject2) {
    // Add logic for collision prediction along an objects trajectory.
}