#include "tongue.h"

#define TONGUE_VELOCITY 600
#define MAX_TONGUE_LENGTH 160.0f

struct tongue {
    Entity tip;
    Vec2 velocity;
    TongueState state;
    Vec2 mousePosition;
    SDL_FRect shaftRect;
    float angle;
};

Tongue *createTongue(Vec2 position) {
    Tongue *pTongue = malloc(sizeof(Tongue));
    if (entityInitData(&(pTongue->tip), position, ENTITY_TONGUE, HITBOX_FULL_BLOCK)) { return NULL; }

    pTongue->velocity = createVector(0.0f, 0.0f);
    pTongue->state = NEUTRAL;
    pTongue->shaftRect.x = 0.0f;
    pTongue->shaftRect.y = 0.0f;
    pTongue->shaftRect.w = 0.0f;
    pTongue->shaftRect.h = 20.0f;

    return pTongue;
}

void tongueUpdateVelocity(Tongue *pTongue, Vec2 centerPoint) {
    Vec2 tongueVec;
    switch (pTongue->state) {
        case NEUTRAL:
            vectorSub(&tongueVec, pTongue->mousePosition, centerPoint);
            vectorGetAngle(centerPoint, entityGetMidPoint(pTongue->tip));

            vectorNorm(&tongueVec);
        
            vectorScale(&tongueVec, TONGUE_VELOCITY);
            pTongue->velocity = tongueVec;
            pTongue->state = EXTENDING;
            break;
        case EXTENDING:
        case HIT_ENTITY:
            vectorSub(&tongueVec, centerPoint, entityGetMidPoint(pTongue->tip));
            vectorNorm(&tongueVec);
        
            vectorScale(&tongueVec, TONGUE_VELOCITY);
            pTongue->velocity = tongueVec;
            pTongue->state = RETRACTING;
    }

    return;
}

void tongueUpdatePosition(Tongue *pTongue, Vec2 centerPoint, float timestep) {
    Vec2 scaledVelocity = pTongue->velocity;
    vectorScale(&scaledVelocity, timestep);
    entityMove(&(pTongue->tip), scaledVelocity);
    tongueCalculateShaft(pTongue, centerPoint, entityGetMidPoint(pTongue->tip));
    switch (pTongue->state) {
        case EXTENDING:
            if (pTongue->shaftRect.w >= MAX_TONGUE_LENGTH) {
                pTongue->shaftRect.w = MAX_TONGUE_LENGTH;
                pTongue->velocity.x = 0.0f;
                pTongue->velocity.y = 0.0f;
                tongueUpdateVelocity(pTongue, centerPoint);
            }
            break;
        case RETRACTING:
            if (pTongue->shaftRect.w < 16.0f) {
                entitySetPosition(&(pTongue->tip), centerPoint);
                pTongue->shaftRect.w = 0.0f;
                pTongue->velocity.x = 0.0f;
                pTongue->velocity.y = 0.0f;
                pTongue->state = NEUTRAL;
            }
            break;
    }

    return;
}

void tongueCalculateShaft(Tongue *pTongue, Vec2 centerPoint, Vec2 referencePoint) {
    pTongue->shaftRect.w = vectorLength(centerPoint, referencePoint);
    tongueSetPosition(pTongue, referencePoint);
    Vec2 midPoint;
    vectorMidPoint(&midPoint, centerPoint, referencePoint);
    pTongue->shaftRect.x = midPoint.x;
    pTongue->shaftRect.y = midPoint.y;
    pTongue->angle = vectorGetAngle(centerPoint, referencePoint);
    return;
}

bool tongueCheckCollision(Tongue *pTongue, Entity const entity) {
    Hitbox *pTongueHitbox = pTongue->tip.pHitbox;
    Hitbox *pEntityHitbox = entity.pHitbox;
    if (hitboxCheckCollision(pTongueHitbox, pEntityHitbox)) {
        tongueSetPosition(pTongue, entityGetMidPoint(entity));
        pTongue->state = HIT_ENTITY;
        pTongue->velocity.x = 0.0f;
        pTongue->velocity.y = 0.0f;
        return true;
    }

    return false;
}

bool tongueSetMousePosition(Tongue *pTongue, Vec2 mousePosition) {
    bool mousePositionSet = false;
    switch (pTongue->state) {
        case NEUTRAL:
            pTongue->mousePosition = mousePosition;
            mousePositionSet = true;
            break;
    }

    return mousePositionSet;
}

void tongueSetPosition(Tongue *pTongue, Vec2 newPosition) {
    newPosition.x -= pTongue->tip.frame.w*0.5f;
    newPosition.y -= pTongue->tip.frame.h*0.5f;
    entitySetPosition(&(pTongue->tip), newPosition);
    return;
}

void tongueSetShaftMidPoint(Tongue *pTongue, Vec2 newPosition) {
    pTongue->shaftRect.x = newPosition.x;
    pTongue->shaftRect.y = newPosition.y;
    return;
}

TongueInfo tongueGetInfo(Tongue const *pTongue) {
    TongueInfo info;
    info.position = entityGetMidPoint(pTongue->tip);
    info.velocity = pTongue->velocity;
    info.mousePosition = pTongue->mousePosition;
    info.length = pTongue->shaftRect.w;
    info.angle = pTongue->angle;
    info.state = pTongue->state;
    info.tongueShaft = pTongue->shaftRect;
    return info;
}

Entity tongueGetTip(Tongue const *pTongue) {
    return pTongue->tip;
}

Hitbox *tongueGetHitbox(Tongue const *pTongue) {
    return pTongue->tip.pHitbox;
}

void tongueOverrideVelocity(Tongue *pTongue, Vec2 velocity) {
    pTongue->velocity = velocity;
    return;
}

void destroyTongue(Tongue *pTongue) {
    if (pTongue == NULL) { return; }

    destroyHitbox(pTongue->tip.pHitbox);
    return;
}
