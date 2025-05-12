#pragma once
#include "entity.h"

typedef enum ObstacleType {
    OBSTACLE_SPIKE
} ObstacleType;

typedef struct obstacle Obstacle;

Obstacle *createObstacle(Vec2 position);

Entity obstacleGetBody(Obstacle const *obstacle);

Hitbox *obstacleGetHitbox(Obstacle const *obstacle);

void destroyObstacle(Obstacle *obstacle);