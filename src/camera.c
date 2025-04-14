#include "camera.h"

#define TRACKING_TIMER 6

typedef struct {
    int width;
    int height;
    int refreshRate;
} Display;

struct camera {
    Vec2 position;
    Vec2 velocity;
    Display display;
    int logicalWidth;
    int logicalHeight;
    SDL_Renderer *pRenderer;
    Entity *pTarget1;
    Entity *pTarget2;
    int cameraMode;
    int trackTimer;
};

Camera *createCamera(int width, int height, int refreshRate, int cameraMode) {
    Camera *pCamera = malloc(sizeof(Camera));
    pCamera->position = createVector(0.0f, 0.0f);
    pCamera->velocity = createVector(0.0f, 0.0f);

    pCamera->display.width = width;
    pCamera->display.height = height;
    pCamera->display.refreshRate = refreshRate;

    pCamera->logicalWidth = 0;
    pCamera->logicalHeight = 0;
    pCamera->pRenderer = NULL;

    pCamera->pTarget1 = NULL;
    pCamera->pTarget2 = NULL;

    pCamera->cameraMode = cameraMode;
    pCamera->trackTimer = TRACKING_TIMER;

    return pCamera;
}

int cameraSetRenderer(Camera *pCamera, SDL_Renderer *pRenderer) {
    if (pCamera == NULL) { return CAMERA_IS_NULL; }

    pCamera->pRenderer = pRenderer;
    return 0;
}

int cameraSetTargets(Camera *pCamera, Entity *pTarget1, Entity *pTarget2) {
    if (pCamera == NULL) { return CAMERA_IS_NULL; }

    pCamera->pTarget1 = pTarget1;
    pCamera->pTarget2 = pTarget2;
    return 0;
}

int cameraSetMode(Camera *pCamera, int cameraMode) {
    if (pCamera == NULL) { return CAMERA_IS_NULL; }

    pCamera->cameraMode = cameraMode;
    return 0;
}

int cameraSetZoom(Camera *pCamera, float zoomScale) {
    if (pCamera == NULL) { return CAMERA_IS_NULL; }

    if (zoomScale > MAX_ZOOM_IN) { zoomScale = MAX_ZOOM_IN; }
    else if (zoomScale < MAX_ZOOM_OUT) { zoomScale = MAX_ZOOM_OUT; }

    pCamera->logicalWidth = round(pCamera->display.width / zoomScale);
    pCamera->logicalHeight = round(pCamera->display.height / zoomScale);
    SDL_RenderSetLogicalSize(pCamera->pRenderer, pCamera->logicalWidth, pCamera->logicalHeight);
    return 0;
}

void cameraScaleToTargets(Camera *pCamera) {
    if (pCamera == NULL) { return; }

    Vec2 vector1 = createVector(getCurrentFrame(pCamera->pTarget1).x, getCurrentFrame(pCamera->pTarget1).y);
    Vec2 vector2 = createVector(getCurrentFrame(pCamera->pTarget2).x, getCurrentFrame(pCamera->pTarget2).y);
    Vec2 middlePoint;
    vectorMidPoint(&middlePoint, vector1, vector2);

    pCamera->position.x = middlePoint.x;
    pCamera->position.y = middlePoint.y;

    Vec2 difference;
    vectorSub(&difference, vector1, vector2);

    float zoomX = pCamera->display.width/(fabsf(difference.x)*1.5f);
    float zoomY = pCamera->display.height/(fabsf(difference.y)*1.5f);
    float zoomToApply;

    if (zoomX < zoomY) { zoomToApply = zoomX; }
    else { zoomToApply = zoomY; }

    if (zoomToApply > MAX_ZOOM_IN) { zoomToApply = MAX_ZOOM_IN; }
    else if (zoomToApply < MAX_ZOOM_OUT) { zoomToApply = MAX_ZOOM_OUT; }

    cameraSetZoom(pCamera, zoomToApply);

    return;
}

void cameraTrackTarget(Camera *pCamera, Vec2 referencePosition) {
    if (pCamera == NULL) { return; }

    if (pCamera->trackTimer > 0) {
        vectorAdd(&pCamera->position, pCamera->position, pCamera->velocity);
        pCamera->trackTimer--;

        if (pCamera->trackTimer == 0) {
            Vec2 distance;
            vectorSub(&distance, referencePosition, pCamera->position);
            pCamera->velocity.x = distance.x;
            if (fabsf(distance.y) > 16.0f && fabsf(distance.y) < 32.0f) { pCamera->velocity.y = distance.y*0.5f; }
            else { pCamera->velocity.y = distance.y; }
            float scalar = 1.0f/((TRACKING_TIMER*pCamera->display.refreshRate/60)*1.75f);
            vectorScale(&pCamera->velocity, scalar);
            
            pCamera->trackTimer = TRACKING_TIMER;
        }
    }
    
    return;
}

int cameraUpdate(Camera *pCamera) {
    if (pCamera == NULL) { return CAMERA_IS_NULL; }

    if (pCamera->pRenderer == NULL) {
        printf("Error: Camera does not have a SDL_Renderer*\n");
        return CAMERA_MISSING_RENDERER;
    }

    switch (pCamera->cameraMode) {
        case CAMERA_SCALING:
            if (pCamera->pTarget1 == NULL && pCamera->pTarget2 == NULL) { return CAMERA_MISSING_TARGETS; }
            cameraScaleToTargets(pCamera);
            break;
        case CAMERA_TRACKING_T1:
            if (pCamera->pTarget1 == NULL) { return CAMERA_MISSING_TARGET1; }
            cameraTrackTarget(pCamera, getPosition(pCamera->pTarget1));
            break;
        case CAMERA_TRACKING_T2:
            if (pCamera->pTarget2 == NULL) { return CAMERA_MISSING_TARGET2; }
            cameraTrackTarget(pCamera, getPosition(pCamera->pTarget2));
            break;
    }
    
    return 0;
}

bool entityIsVisible(Camera const *pCamera, Entity const *pEntity) {
    bool isVisible = true;
    SDL_FRect entityFRect = getCurrentFrame(pEntity);
    float deltaDistanceX = fabsf(pCamera->position.x-entityFRect.x);
    float deltaDistanceY = fabsf(pCamera->position.y-entityFRect.y);

    if (deltaDistanceX > pCamera->logicalWidth*0.5f+entityFRect.w) {
        isVisible = false;
    }
    if (deltaDistanceY > pCamera->logicalHeight*0.5f+entityFRect.h) {
        isVisible = false;
    }

    return isVisible;
}

int cameraGetWidth(Camera const *pCamera) {
    if (pCamera == NULL) { return CAMERA_IS_NULL; }

    return pCamera->logicalWidth;
}

int cameraGetHeight(Camera const *pCamera) {
    if (pCamera == NULL) { return CAMERA_IS_NULL; }

    return pCamera->logicalHeight;
}

Vec2 cameraGetPosition(Camera const *pCamera) {
    if (pCamera == NULL) { return createVector(0.0f, 0.0f); }

    return pCamera->position;
}

int cameraGetMode(Camera const *pCamera) {
    if (pCamera == NULL) { return CAMERA_IS_NULL; }

    return pCamera->cameraMode;
}

void destroyCamera(Camera *pCamera) {
    if (pCamera == NULL) { return; }

    free(pCamera);
}