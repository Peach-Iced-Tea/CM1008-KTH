#include "tongue.h"

#define TONGUE_VELOCITY 600
#define MAX_TONGUE_LENGTH 160.0f

struct tongue {
    Entity tip;
    Entity shaft;
    Vec2 velocity;
    Vec2 mousePosition;
    TongueState state;
};

Tongue *createTongue(Vec2 position) {
    Tongue *pTongue = malloc(sizeof(Tongue));
    if (entityInitData(&(pTongue->tip), position, ENTITY_TONGUE_TIP, HITBOX_FULL_BLOCK)) { return NULL; }
    pTongue->tip.source.x = 32;

    if (entityInitData(&(pTongue->shaft), createVector(0.0f, 0.0f), ENTITY_TONGUE_SHAFT, HITBOX_NONE)) { return NULL; }

    pTongue->velocity = createVector(0.0f, 0.0f);
    pTongue->mousePosition = createVector(0.0f, 0.0f);
    pTongue->state = NEUTRAL;

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
            if (pTongue->shaft.frame.w >= MAX_TONGUE_LENGTH) {
                pTongue->shaft.frame.w = MAX_TONGUE_LENGTH;
                pTongue->velocity.x = 0.0f;
                pTongue->velocity.y = 0.0f;
                tongueUpdateVelocity(pTongue, centerPoint);
            }
            break;
        case RETRACTING:
            if (pTongue->shaft.frame.w < 16.0f) {
                entitySetPosition(&(pTongue->tip), centerPoint);
                pTongue->shaft.frame.w = 0.0f;
                pTongue->velocity.x = 0.0f;
                pTongue->velocity.y = 0.0f;
                pTongue->state = NEUTRAL;
            }
            break;
    }

    return;
}

void tongueCalculateShaft(Tongue *pTongue, Vec2 centerPoint, Vec2 referencePoint) {
    pTongue->shaft.frame.w = vectorLength(centerPoint, referencePoint);
    tongueSetPosition(pTongue, referencePoint);
    Vec2 midPoint;
    vectorMidPoint(&midPoint, centerPoint, referencePoint);
    pTongue->shaft.frame.x = midPoint.x - pTongue->shaft.frame.w*0.5f;
    pTongue->shaft.frame.y = midPoint.y - pTongue->shaft.frame.h*0.5f;
    pTongue->shaft.angle = vectorGetAngle(centerPoint, referencePoint) * (180.0f/M_PI);
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

TongueInfo tongueGetInfo(Tongue const *pTongue) {
    TongueInfo info;
    info.position = entityGetMidPoint(pTongue->tip);
    info.velocity = pTongue->velocity;
    info.mousePosition = pTongue->mousePosition;
    info.length = pTongue->shaft.frame.w;
    info.state = pTongue->state;
    return info;
}

Entity tongueGetTip(Tongue const *pTongue) {
    return pTongue->tip;
}

Entity tongueGetShaft(Tongue const *pTongue) {
    return pTongue->shaft;
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
