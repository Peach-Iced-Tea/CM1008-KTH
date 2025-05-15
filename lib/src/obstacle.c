#include "obstacle.h"

struct obstacle {
    Entity body;
    HazardLevel hazardLevel;
    ObstacleType type;
};

Obstacle *createObstacle(Vec2 position, ObstacleType type) {
    Obstacle *pObstacle = malloc(sizeof(Obstacle));
    pObstacle->hazardLevel = HAZARD_NONE;
    pObstacle->type = type;
    switch (pObstacle->type) {
        case OBSTACLE_SPIKE:
            if (entityInitData(&(pObstacle->body), position, ENTITY_DEFAULT, HITBOX_HALF_BLOCK_BOTTOM)) { return NULL; }
            pObstacle->body.source.x = 0;
            pObstacle->body.source.y = 0;
            pObstacle->hazardLevel = HAZARD_LETHAL;
            break;
        case OBSTACLE_CHECKPOINT:
            if (entityInitData(&(pObstacle->body), position, ENTITY_DEFAULT, HITBOX_FULL_BLOCK)) { return NULL; }
            break;
        default:
            printf("Error: Unknown ObstacleType %d", type);
            return NULL;
    }

    return pObstacle;
}

HazardLevel obstacleIsHazardous(Obstacle const *pObstacle) {
    return pObstacle->hazardLevel;
}

Entity obstacleGetBody(Obstacle const *pObstacle) {
    return pObstacle->body;
}

Vec2 obstacleGetPosition(Obstacle const *pObstacle) {
    return entityGetPosition(pObstacle->body);
}

Hitbox *obstacleGetHitbox(Obstacle const *pObstacle) {
    return pObstacle->body.pHitbox;
}

void destroyObstacle(Obstacle *pObstacle) {
    if (pObstacle == NULL) { return; }

    free(pObstacle);
    return;
}