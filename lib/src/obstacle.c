#include "obstacle.h"

struct obstacle {
    Entity body;
};

Obstacle *createObstacle(Vec2 position) {
    Obstacle *pObstacle = malloc(sizeof(Obstacle));
    if (entityInitData(&(pObstacle->body), position, ENTITY_DEFAULT, HITBOX_HALF_BLOCK_BOTTOM)) { return NULL; }

    return pObstacle;
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