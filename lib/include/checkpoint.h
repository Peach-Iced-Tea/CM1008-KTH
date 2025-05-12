#pragma once
#include "entity.h"

typedef struct checkpoint Checkpoint;

Checkpoint *createCheckpoint(Vec2 position);

Entity checkpointGetBody(Checkpoint const *checkpoint);

Vec2 checkpointGetPosition(Checkpoint const *checkpoint);

Hitbox *checkpointGetHitbox(Checkpoint const *checkpoint);

void destroyCheckpoint(Checkpoint *checkpoint);