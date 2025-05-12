#include "checkpoint.h"

struct checkpoint {
    Entity body;
};

Checkpoint *createCheckpoint(Vec2 position) {
    Checkpoint *pCheckpoint = malloc(sizeof(Checkpoint));
    if (entityInitData(&(pCheckpoint->body), position, ENTITY_DEFAULT, HITBOX_FULL_BLOCK)) { return NULL; }

    return pCheckpoint;
}

Entity checkpointGetBody(Checkpoint const *pCheckpoint) {
    return pCheckpoint->body;
}

Vec2 checkpointGetPosition(Checkpoint const *pCheckpoint) {
    return entityGetPosition(pCheckpoint->body);
}

Hitbox *checkpointGetHitbox(Checkpoint const *pCheckpoint) {
    return pCheckpoint->body.pHitbox;
}

void destroyCheckpoint(Checkpoint *pCheckpoint) {
    if (pCheckpoint == NULL) { return; }

    free(pCheckpoint);
    return;
}
