#include "tongue.h"

#define TONGUE_VELOCITY 600
#define MAX_TONGUE_LENGTH 256.0f

struct tongue {
    Entity *pTip;
    SDL_Texture *pTongueTexture;
    Vec2 velocity;
    TongueState state;
    Vec2 mousePosition;
    SDL_FRect shaftRect;
    float angle;
};

Tongue *createTongue(Vec2 position, SDL_Renderer *pRenderer) {
    Tongue *pTongue = malloc(sizeof(Tongue));
    SDL_Texture *pTexture = IMG_LoadTexture(pRenderer, "lib/resources/tongueTip.png");
    pTongue->pTip = createEntity(position, pTexture, ENTITY_TONGUE, HITBOX_FULL_BLOCK);
    if (pTongue->pTip == NULL) { return NULL; }

    pTongue->pTongueTexture = IMG_LoadTexture(pRenderer, "lib/resources/tongueShaft.png");
    pTongue->velocity = createVector(0.0f, 0.0f);
    pTongue->state = NEUTRAL;
    pTongue->shaftRect.x = 0.0f;
    pTongue->shaftRect.y = 0.0f;
    pTongue->shaftRect.w = 0.0f;
    pTongue->shaftRect.h = 20.0f;

    return pTongue;
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
    newPosition.x -= entityGetCurrentFrame(pTongue->pTip).w*0.5f;
    newPosition.y -= entityGetCurrentFrame(pTongue->pTip).h*0.5f;
    entitySetPosition(pTongue->pTip, newPosition);
    return;
}

void tongueSetShaftMidPoint(Tongue *pTongue, Vec2 newPosition) {
    pTongue->shaftRect.x = newPosition.x;
    pTongue->shaftRect.y = newPosition.y;
    return;
}

void tongueSetVelocity(Tongue *pTongue, Vec2 centerPoint) {
    Vec2 tongueVec;
    switch (pTongue->state) {
        case NEUTRAL:
            vectorSub(&tongueVec, pTongue->mousePosition, centerPoint);
            vectorGetAngle(centerPoint, entityGetMidPoint(pTongue->pTip));

            vectorNorm(&tongueVec);
        
            vectorScale(&tongueVec, TONGUE_VELOCITY);
            pTongue->velocity = tongueVec;
            pTongue->state = EXTENDING;
            break;
        case EXTENDING:
        case MAX_EXTENSION:
            vectorSub(&tongueVec, centerPoint, entityGetMidPoint(pTongue->pTip));
            vectorNorm(&tongueVec);
        
            vectorScale(&tongueVec, TONGUE_VELOCITY);
            pTongue->velocity = tongueVec;
            pTongue->state = RETRACTING;
    }

    return;
}

void tongueUpdate(Tongue *pTongue, Vec2 centerPoint, float timestep) {
    if (pTongue->state == EXTENDING || pTongue->state == RETRACTING) {
        Vec2 scaledVelocity = pTongue->velocity;
        vectorScale(&scaledVelocity, timestep);
        entityMove(pTongue->pTip, scaledVelocity);
    }

    tongueCalculateShaft(pTongue, centerPoint, entityGetMidPoint(pTongue->pTip));
    switch (pTongue->state) {
        case EXTENDING:
            if (pTongue->shaftRect.w >= MAX_TONGUE_LENGTH) {
                pTongue->shaftRect.w = MAX_TONGUE_LENGTH;
                pTongue->velocity.x = 0.0f;
                pTongue->velocity.y = 0.0f;
                pTongue->state = MAX_EXTENSION;
            }
            break;
        case RETRACTING:
            if (pTongue->shaftRect.w < 16.0f) {
                entitySetPosition(pTongue->pTip, centerPoint);
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

bool tongueCheckCollision(Tongue *pTongue, Entity *pEntity) {
    Hitbox *pTongueHitbox = entityGetHitbox(pTongue->pTip);
    Hitbox *pEntityHitbox = entityGetHitbox(pEntity);
    if (checkCollision(pTongueHitbox, pEntityHitbox)) {
        tongueSetPosition(pTongue, entityGetMidPoint(pEntity));
        pTongue->velocity.x = 0.0f;
        pTongue->velocity.y = 0.0f;
        return true;
    }

    return false;
}

TongueInfo tongueGetInfo(Tongue const *pTongue) {
    TongueInfo info;
    info.position = entityGetMidPoint(pTongue->pTip);
    info.velocity = pTongue->velocity;
    info.mousePosition = pTongue->mousePosition;
    info.length = pTongue->shaftRect.w;
    info.angle = pTongue->angle;
    info.state = pTongue->state;
    info.tongueShaft = pTongue->shaftRect;
    return info;
}

Entity *tongueGetTip(Tongue const *pTongue) {
    return pTongue->pTip;
}

Hitbox *tongueGetHitbox(Tongue const *pTongue) {
    return entityGetHitbox(pTongue->pTip);
}

SDL_Texture *tongueGetShaftTexture(Tongue const *pTongue) {
    return pTongue->pTongueTexture;
}

void destroyTongue(Tongue *pTongue) {
    if (pTongue == NULL) { return; }

    destroyEntity(pTongue->pTip);
    SDL_DestroyTexture(pTongue->pTongueTexture);
    return;
}
