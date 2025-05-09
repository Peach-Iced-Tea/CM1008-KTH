#include "mouse.h"

#define CROSSHAIR_MAX_DISTANCE 160.0f

struct crosshair {
    Entity body;
    Vec2 relativePosition;
    Vec2 velocity;
};

Crosshair *createCrosshair(Vec2 const position) {
    Crosshair *pCrosshair = malloc(sizeof(Crosshair));
    if (entityInitData(&(pCrosshair->body), position, ENTITY_CROSSHAIR, HITBOX_FULL_BLOCK)) { return NULL; }
    
    pCrosshair->relativePosition = createVector(0.0f, 0.0f);
    pCrosshair->velocity = createVector(0.0f, 0.0f);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    return pCrosshair;
}

void crosshairHandleInput(Crosshair *pCrosshair, Input *pInput) {
    pCrosshair->velocity = createVector((float)getMouseState(pInput, MOUSE_MOTION_X), (float)getMouseState(pInput, MOUSE_MOTION_Y));
    vectorAdd(&(pCrosshair->relativePosition), pCrosshair->relativePosition, pCrosshair->velocity);
    Vec2 position = entityGetMidPoint(pCrosshair->body);
    if (pCrosshair->relativePosition.x < -CROSSHAIR_MAX_DISTANCE) {
        pCrosshair->relativePosition.x = -CROSSHAIR_MAX_DISTANCE;
    }
    else if (pCrosshair->relativePosition.x > CROSSHAIR_MAX_DISTANCE) {
        pCrosshair->relativePosition.x = CROSSHAIR_MAX_DISTANCE;
    }

    if (pCrosshair->relativePosition.y < -CROSSHAIR_MAX_DISTANCE) {
        pCrosshair->relativePosition.y = -CROSSHAIR_MAX_DISTANCE;
    }
    else if (pCrosshair->relativePosition.y > CROSSHAIR_MAX_DISTANCE) {
        pCrosshair->relativePosition.y = CROSSHAIR_MAX_DISTANCE;
    }

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