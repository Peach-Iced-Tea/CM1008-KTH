#include "networkData.h"

void prepareInputData(InputData *pData, Player const *pPlayer, int tick) {
    PlayerInfo info = playerGetInfo(pPlayer);
    pData->input = info.velocity;
    pData->mouseAim = tongueGetInfo(playerGetTongue(pPlayer)).mousePosition;
    pData->rotateVelocity = info.rotateVelocity;
    pData->state = info.state;
    return;
}

void prepareStateData(StateData *pData, Player const *pPlayer, int tick) {
    PlayerInfo info = playerGetInfo(pPlayer);
    pData->position = info.position;
    pData->tonguePosition = tongueGetInfo(playerGetTongue(pPlayer)).position;
    pData->state = info.state;
    pData->sheetPosition.x = info.sheetPosition.x;
    pData->sheetPosition.y = info.sheetPosition.y;
    pData->sheetFlip = playerGetBody(pPlayer).flip;
    if (tick >= 0) {
        pData->tick = tick;
    }
    return;
}

void prepareEntityData(EntityData *pData, Entity const *pEntity, int entityID, int tick) {
    if (pEntity != NULL) {
        pData->position = entityGetPosition(*pEntity);
    }
    else {
        pData->position = createVector(0.0f, 0.0f);
    }
    pData->entityID = entityID;
    if (tick >= 0) {
        pData->tick = tick;
    }
    return;
}