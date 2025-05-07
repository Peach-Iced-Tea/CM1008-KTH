#include "checkpoints.h"

struct checkpoint {
    Vec2 position;
    int checkpointID;
};

void gotoCheckpoint(Player *pPlayer){
    Checkpoint *pCheckpoint = playerGetCheckpoint(pPlayer);
    playerSetPosition(pPlayer, pCheckpoint->position);
}

void setNewCheckpoint(Player *pPlayer, Checkpoint *newCheckpoint){
    playerSetCheckpoint(pPlayer, newCheckpoint);
}