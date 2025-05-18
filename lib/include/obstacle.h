#pragma once
#include "vmath.h"
#include "entity.h"

typedef enum ObstacleType {
    OBSTACLE_SPIKE, OBSTACLE_CHECKPOINT
} ObstacleType;

typedef enum HazardLevel {
    HAZARD_NONE, HAZARD_LETHAL
} HazardLevel;

typedef struct obstacle Obstacle;

Obstacle *createObstacle(Vec2 position, ObstacleType type);

HazardLevel obstacleIsHazardous(Obstacle const *pObstacle);

Entity obstacleGetBody(Obstacle const *obstacle);

Vec2 obstacleGetPosition(Obstacle const *pObstacle);

Hitbox *obstacleGetHitbox(Obstacle const *obstacle);

void destroyObstacle(Obstacle *obstacle);