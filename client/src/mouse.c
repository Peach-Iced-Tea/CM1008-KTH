#include "mouse.h"

#define CROSSHAIR_WIDTH 32
#define CROSSHAIR_HEIGHT 32
#define CROSSHAIR_MAX_DISTANCE 192.0f

struct crosshair {
    Entity *pBody;
    Vec2 relativePosition;
    Vec2 velocity;
    SDL_Rect sheetPosition;
};

Crosshair *createCrosshair(SDL_Renderer *pRenderer, Vec2 const position) {
    Crosshair *pCrosshair = malloc(sizeof(Crosshair));
    pCrosshair->pBody = createEntity(position, NULL, ENTITY_CROSSHAIR, HITBOX_FULL_BLOCK);
    pCrosshair->relativePosition = createVector(0.0f, 0.0f);
    pCrosshair->velocity = createVector(0.0f, 0.0f);

    pCrosshair->sheetPosition.x = 0;
    pCrosshair->sheetPosition.y = 0;
    pCrosshair->sheetPosition.w = CROSSHAIR_WIDTH;
    pCrosshair->sheetPosition.h = CROSSHAIR_HEIGHT;
    SDL_SetRelativeMouseMode(SDL_TRUE);

    return pCrosshair;
}

void crosshairHandleInput(Crosshair *pCrosshair, Input *pInput) {
    pCrosshair->velocity = createVector((float)getMouseState(pInput, MOUSE_MOTION_X), (float)getMouseState(pInput, MOUSE_MOTION_Y));
    vectorAdd(&(pCrosshair->relativePosition), pCrosshair->relativePosition, pCrosshair->velocity);
    Vec2 position = entityGetMidPoint(pCrosshair->pBody);
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

    pCrosshair->sheetPosition.x = 0;
    pCrosshair->sheetPosition.y = 0;
    if (getMouseState(pInput, MOUSE_LEFT)) {
        pCrosshair->sheetPosition.x += pCrosshair->sheetPosition.w;
    }

    if (getMouseState(pInput, MOUSE_RIGHT)) {
        pCrosshair->sheetPosition.y += pCrosshair->sheetPosition.h;
    }

    return;
}

void crosshairUpdatePosition(Crosshair *pCrosshair, Vec2 referencePosition) {
    Vec2 position;
    vectorAdd(&position, referencePosition, pCrosshair->relativePosition);
    entitySetPosition(pCrosshair->pBody, position);
    return;
}

Entity *crosshairGetBody(Crosshair const *pCrosshair) {
    return pCrosshair->pBody;
}

Vec2 crosshairGetPosition(Crosshair const *pCrosshair) {
    return entityGetMidPoint(pCrosshair->pBody);
}

SDL_Rect crosshairGetSheetPosition(Crosshair const *pCrosshair) {
    return pCrosshair->sheetPosition;
}

void destroyCrosshair(Crosshair *pCrosshair) {
    if (pCrosshair == NULL) { return; }

    destroyEntity(pCrosshair->pBody);
    free(pCrosshair);

    return;
}