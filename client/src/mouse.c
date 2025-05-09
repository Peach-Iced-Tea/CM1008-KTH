#include "mouse.h"

#define CROSSHAIR_MAX_DISTANCE 160.0f

struct crosshair {
    Entity body;
    Vec2 relativePosition;
    Vec2 velocity;
    Vec2 borders;
    bool lockPosition;
};

Crosshair *createCrosshair(Vec2 const position) {
    Crosshair *pCrosshair = malloc(sizeof(Crosshair));
    if (entityInitData(&(pCrosshair->body), position, ENTITY_CROSSHAIR, HITBOX_FULL_BLOCK)) { return NULL; }
    
    pCrosshair->relativePosition = createVector(0.0f, 0.0f);
    pCrosshair->velocity = createVector(0.0f, 0.0f);
    pCrosshair->borders = createVector(0.0f, 0.0f);
    pCrosshair->lockPosition = false;
    SDL_SetRelativeMouseMode(SDL_TRUE);

    return pCrosshair;
}

void crosshairHandleInput(Crosshair *pCrosshair, Input *pInput) {
    pCrosshair->velocity = createVector((float)getMouseState(pInput, MOUSE_MOTION_X), (float)getMouseState(pInput, MOUSE_MOTION_Y));
    vectorAdd(&(pCrosshair->relativePosition), pCrosshair->relativePosition, pCrosshair->velocity);
    if (pCrosshair->relativePosition.x < -pCrosshair->borders.x) {
        pCrosshair->relativePosition.x = -pCrosshair->borders.x;
    }
    else if (pCrosshair->relativePosition.x > pCrosshair->borders.x-pCrosshair->body.frame.w) {
        pCrosshair->relativePosition.x = pCrosshair->borders.x-pCrosshair->body.frame.w;
    }

    if (pCrosshair->relativePosition.y < -pCrosshair->borders.y-pCrosshair->body.frame.h*0.5f) {
        pCrosshair->relativePosition.y = -pCrosshair->borders.y-pCrosshair->body.frame.h*0.5f;
    }
    else if (pCrosshair->relativePosition.y > pCrosshair->borders.y-pCrosshair->body.frame.h*0.5f) {
        pCrosshair->relativePosition.y = pCrosshair->borders.y-pCrosshair->body.frame.h*0.5f;
    }

    pCrosshair->lockPosition = false;
    if (pCrosshair->velocity.x == 0.0f && pCrosshair->velocity.y == 0.0f) { pCrosshair->lockPosition = true; }

    pCrosshair->body.source.x = 0;
    pCrosshair->body.source.y = 0;
    if (getMouseState(pInput, MOUSE_LEFT)) {
        pCrosshair->body.source.x += pCrosshair->body.source.w;
    }

    if (getMouseState(pInput, MOUSE_RIGHT)) {
        pCrosshair->body.source.y += pCrosshair->body.source.h;
    }

    return;
}

void crosshairUpdatePosition(Crosshair *pCrosshair, Vec2 referencePosition) {
    Vec2 position;
    vectorAdd(&position, referencePosition, pCrosshair->relativePosition);
    entitySetPosition(&(pCrosshair->body), position);
    return;
}

void crosshairSetBorders(Crosshair *pCrosshair, float borderX, float borderY) {
    pCrosshair->borders.x = borderX*0.5f;
    pCrosshair->borders.y = borderY*0.5f - pCrosshair->body.frame.h*0.5f;
    return;
}

Entity crosshairGetBody(Crosshair const *pCrosshair) {
    return pCrosshair->body;
}

Vec2 crosshairGetPosition(Crosshair const *pCrosshair) {
    return entityGetMidPoint(pCrosshair->body);
}

void destroyCrosshair(Crosshair *pCrosshair) {
    if (pCrosshair == NULL) { return; }

    destroyHitbox(pCrosshair->body.pHitbox);
    free(pCrosshair);

    return;
}