#include "mouse.h"

#define Mouse_MAX_DISTANCE 160.0f

struct mouse {
    Entity body;
    Vec2 relativePosition;
    Vec2 velocity;
    Vec2 borders;
    bool lockPosition;
};

Mouse *createMouse(Vec2 const position) {
    Mouse *pMouse = malloc(sizeof(Mouse));
    if (entityInitData(&(pMouse->body), position, ENTITY_CROSSHAIR, HITBOX_FULL_BLOCK)) { return NULL; }
    
    pMouse->relativePosition = createVector(0.0f, 0.0f);
    pMouse->velocity = createVector(0.0f, 0.0f);
    pMouse->borders = createVector(0.0f, 0.0f);
    pMouse->lockPosition = false;
    SDL_SetRelativeMouseMode(SDL_TRUE);

    return pMouse;
}

void mouseHandleInput(Mouse *pMouse, Input *pInput) {
    pMouse->velocity = createVector((float)getMouseState(pInput, MOUSE_MOTION_X), (float)getMouseState(pInput, MOUSE_MOTION_Y));
    vectorAdd(&(pMouse->relativePosition), pMouse->relativePosition, pMouse->velocity);
    if (pMouse->relativePosition.x < -pMouse->borders.x) {
        pMouse->relativePosition.x = -pMouse->borders.x;
    }
    else if (pMouse->relativePosition.x > pMouse->borders.x-pMouse->body.frame.w) {
        pMouse->relativePosition.x = pMouse->borders.x-pMouse->body.frame.w;
    }

    if (pMouse->relativePosition.y < -pMouse->borders.y-pMouse->body.frame.h*0.5f) {
        pMouse->relativePosition.y = -pMouse->borders.y-pMouse->body.frame.h*0.5f;
    }
    else if (pMouse->relativePosition.y > pMouse->borders.y-pMouse->body.frame.h*0.5f) {
        pMouse->relativePosition.y = pMouse->borders.y-pMouse->body.frame.h*0.5f;
    }

    pMouse->lockPosition = false;
    if (pMouse->velocity.x == 0.0f && pMouse->velocity.y == 0.0f) { pMouse->lockPosition = true; }

    pMouse->body.source.x = 0;
    pMouse->body.source.y = 0;
    if (getMouseState(pInput, MOUSE_LEFT)) {
        pMouse->body.source.x += pMouse->body.source.w;
    }

    if (getMouseState(pInput, MOUSE_RIGHT)) {
        pMouse->body.source.y += pMouse->body.source.h;
    }

    return;
}

void mouseUpdatePosition(Mouse *pMouse, Vec2 referencePosition) {
    Vec2 position;
    vectorAdd(&position, referencePosition, pMouse->relativePosition);
    entitySetPosition(&(pMouse->body), position);
    return;
}

void mouseSetBorders(Mouse *pMouse, float borderX, float borderY) {
    pMouse->borders.x = borderX*0.5f;
    pMouse->borders.y = borderY*0.5f - pMouse->body.frame.h*0.5f;
    return;
}

Entity mouseGetBody(Mouse const *pMouse) {
    return pMouse->body;
}

Vec2 mouseGetPosition(Mouse const *pMouse) {
    return entityGetMidPoint(pMouse->body);
}

void destroyMouse(Mouse *pMouse) {
    if (pMouse == NULL) { return; }

    destroyHitbox(pMouse->body.pHitbox);
    free(pMouse);

    return;
}