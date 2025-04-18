#include "camera.h"

#define TRACKING_TIMER 6

typedef struct {
    int trueWidth;
    int trueHeight;
    int adjustedWidth;
    int adjustedHeight;
    float maxZoomOut;
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
    CameraMode mode;
    float currentZoom;
    int trackTimer;
};

Camera *createCamera(int width, int height, int refreshRate, int cameraMode) {
    Camera *pCamera = malloc(sizeof(Camera));
    pCamera->position = createVector(0.0f, 0.0f);
    pCamera->velocity = createVector(0.0f, 0.0f);

    pCamera->display.trueWidth = width;
    pCamera->display.trueHeight = height;
    if (width > MAX_LOGICAL_WIDTH) {
        pCamera->display.adjustedWidth = MAX_LOGICAL_WIDTH;
        pCamera->display.adjustedHeight = MAX_LOGICAL_WIDTH * height/width;
    }
    else {
        pCamera->display.adjustedWidth = width;
        pCamera->display.adjustedHeight = height;
    }

    pCamera->display.maxZoomOut = (float)width/MAX_LOGICAL_WIDTH;
    pCamera->display.refreshRate = refreshRate;

    pCamera->logicalWidth = 0;
    pCamera->logicalHeight = 0;
    pCamera->pRenderer = NULL;

    pCamera->pTarget1 = NULL;
    pCamera->pTarget2 = NULL;

    pCamera->mode = cameraMode;
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

int cameraSetMode(Camera *pCamera, int newMode) {
    if (pCamera == NULL) { return CAMERA_IS_NULL; }

    switch (newMode) {
        case SCALING:
        case TRACKING_T1:
        case TRACKING_T2:
        case FIXED:
            break;
    }

    pCamera->mode = newMode;
    
    return 0;
}

int cameraSetZoom(Camera *pCamera, float zoomScale) {
    if (pCamera == NULL) { return CAMERA_IS_NULL; }

    if (zoomScale > MAX_ZOOM_IN) { zoomScale = MAX_ZOOM_IN; }
    else if (zoomScale < pCamera->display.maxZoomOut) { zoomScale = pCamera->display.maxZoomOut; }

    pCamera->logicalWidth = round(pCamera->display.adjustedWidth / zoomScale);
    pCamera->logicalHeight = round(pCamera->display.adjustedHeight / zoomScale);
    pCamera->currentZoom = zoomScale;
    SDL_RenderSetLogicalSize(pCamera->pRenderer, pCamera->logicalWidth, pCamera->logicalHeight);
    return 0;
}

void cameraScaleToTargets(Camera *pCamera) {
    if (pCamera == NULL) { return; }

    Vec2 position1 = getMidPoint(pCamera->pTarget1);
    Vec2 position2 = getMidPoint(pCamera->pTarget2);
    Vec2 middlePoint;
    vectorMidPoint(&middlePoint, position1, position2);

    pCamera->position.x = middlePoint.x;
    pCamera->position.y = middlePoint.y;

    Vec2 difference;
    vectorSub(&difference, position1, position2);

    float zoomX = pCamera->display.adjustedWidth/(fabsf(difference.x)*1.5f);
    float zoomY = pCamera->display.adjustedHeight/(fabsf(difference.y)*1.5f);
    float zoomToApply;

    if (zoomX < zoomY) { zoomToApply = zoomX; }
    else { zoomToApply = zoomY; }

    if (zoomToApply > MAX_ZOOM_IN) { zoomToApply = MAX_ZOOM_IN; }
    else if (zoomToApply < pCamera->display.maxZoomOut) { zoomToApply = pCamera->display.maxZoomOut; }

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

    switch (pCamera->mode) {
        case SCALING:
            if (pCamera->pTarget1 == NULL && pCamera->pTarget2 == NULL) { return CAMERA_MISSING_TARGETS; }
            cameraScaleToTargets(pCamera);
            break;
        case TRACKING_T1:
            if (pCamera->pTarget1 == NULL) { return CAMERA_MISSING_TARGET1; }
            cameraTrackTarget(pCamera, getMidPoint(pCamera->pTarget1));
            break;
        case TRACKING_T2:
            if (pCamera->pTarget2 == NULL) { return CAMERA_MISSING_TARGET2; }
            cameraTrackTarget(pCamera, getMidPoint(pCamera->pTarget2));
            break;
        case FIXED:
            break;
    }
    
    return 0;
}

bool entityIsVisible(Camera const *pCamera, SDL_FRect const entity) {
    bool isVisible = true;
    float deltaDistanceX = fabsf(pCamera->position.x-entity.x);
    float deltaDistanceY = fabsf(pCamera->position.y-entity.y);

    if (deltaDistanceX > pCamera->logicalWidth*0.5f+entity.w) {
        isVisible = false;
    }
    if (deltaDistanceY > pCamera->logicalHeight*0.5f+entity.h) {
        isVisible = false;
    }

    return isVisible;
}

Vec2 cameraGetMousePosition(Camera *pCamera) {
    Vec2 mousePosition = createVector(0.0f, 0.0f);
    int x, y;
    SDL_GetMouseState(&x, &y);
    float actualZoom = ((float)pCamera->display.trueWidth/pCamera->display.adjustedWidth)*pCamera->currentZoom;
    mousePosition.x = (float)x/actualZoom + pCamera->position.x - pCamera->logicalWidth*0.5f;
    mousePosition.y = (float)y/actualZoom + pCamera->position.y - pCamera->logicalHeight*0.5f;
    return mousePosition;
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

    return pCamera->mode;
}

void destroyCamera(Camera *pCamera) {
    if (pCamera == NULL) { return; }

    free(pCamera);
    return;
}