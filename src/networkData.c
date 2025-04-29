#include "networkData.h"

void prepareInputData(InputData *pData, Player *pPlayer, int tick) {
    pData->input = playerGetVelocity(pPlayer);
    pData->tongueInput = tongueGetVelocity(playerGetTongue(pPlayer));
    pData->rotateVelocity = playerGetRotateVelocity(pPlayer);
    pData->state = playerGetState(pPlayer);
    return;
}

void prepareStateData(StateData *pData, Player *pPlayer, int tick) {
    pData->position = playerGetPosition(pPlayer);
    pData->tonguePosition = tongueGetPosition(playerGetTongue(pPlayer));
    pData->state = playerGetState(pPlayer);
    pData->sheetPosition.x = playerGetSheetPosition(pPlayer).x;
    pData->sheetPosition.y = playerGetSheetPosition(pPlayer).y;
    pData->tick = tick;
    return;
}

void prepareEntityData(EntityData *pData, Entity *pEntity, int entityID, int tick) {
    if (pEntity != NULL) {
        pData->position = entityGetPosition(pEntity);
    }
    else {
        pData->position = createVector(0.0f, 0.0f);
    }
    pData->entityID = entityID;
    pData->tick = tick;
    return;
}