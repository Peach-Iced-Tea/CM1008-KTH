#include "camera.h"

#define TRACKING_TIMER 2
#define TRACKING_MODIFIER 5 // This is used to make the camera movement less jumpy when tracking a target.
#define CAMERA_OFFSET_X 32
#define CAMERA_OFFSET_Y 72

typedef struct {
    int width;
    int height;
    int refreshRate;
    float aspectRatio;
    float globalScale;
} Display;

typedef struct {
    int offsetX;
    int offsetY;
    bool followX;
    bool followY;
    int timer;
} Tracker;

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
    Tracker tracker;
    float currentZoom;
};

Camera *createCamera(int width, int height, int refreshRate, int cameraMode) {
    Camera *pCamera = malloc(sizeof(Camera));
    pCamera->position = createVector(0.0f, 0.0f);
    pCamera->velocity = createVector(0.0f, 0.0f);

    pCamera->display.width = width;
    pCamera->display.height = height;
    pCamera->display.refreshRate = refreshRate;
    pCamera->display.aspectRatio = (float)width/height;
    if (width != REFERENCE_WIDTH && height != REFERENCE_HEIGHT) {
        float globalScaleW = (float)width/REFERENCE_WIDTH;
        float globalScaleH = (float)height/REFERENCE_HEIGHT;
        if (globalScaleW <= globalScaleH) {
            pCamera->display.globalScale = globalScaleW;
        }
        else {
            pCamera->display.globalScale = globalScaleH;
        }
    }
    else if (width == REFERENCE_WIDTH || height == REFERENCE_HEIGHT) {
        pCamera->display.globalScale = 1.0f;
    }

    pCamera->logicalWidth = (float)width/MAX_ZOOM_IN;
    pCamera->logicalHeight = (float)height/MAX_ZOOM_IN;
    pCamera->pRenderer = NULL;

    pCamera->pTarget1 = NULL;
    pCamera->pTarget2 = NULL;

    cameraSetMode(pCamera, cameraMode);
    pCamera->currentZoom = MAX_ZOOM_IN;
    pCamera->tracker.timer = TRACKING_TIMER;

    return pCamera;
}

void cameraHandleInput(Camera *pCamera, Input const *pInputs) {
    if (checkKeyCombo(pInputs, KEY_ALT, KEY_1)) { cameraSetMode(pCamera, SCALING); }
    if (checkKeyCombo(pInputs, KEY_ALT, KEY_2)) { cameraSetMode(pCamera, TRACKING_T1); }
    if (checkKeyCombo(pInputs, KEY_ALT, KEY_3)) { cameraSetMode(pCamera, TRACKING_T2); }
    if (getKeyState(pInputs, KEY_ALT) && pCamera->mode != SCALING) {
        if (getKeyState(pInputs, KEY_COMMA)) { cameraSetZoom(pCamera, pCamera->currentZoom-0.025f); }
        if (getKeyState(pInputs, KEY_PERIOD)) { cameraSetZoom(pCamera, pCamera->currentZoom+0.025f); }
    }

    return;
}

int cameraSetRenderer(Camera *pCamera, SDL_Renderer *pRenderer) {
    if (pCamera == NULL) { return IS_NULL; }

    pCamera->pRenderer = pRenderer;
    switch (pCamera->mode) {
        case SCALING:
            break;
        case TRACKING_T1:
        case TRACKING_T2:
        case FIXED:
            SDL_RenderSetLogicalSize(pCamera->pRenderer, pCamera->logicalWidth, pCamera->logicalHeight);
            break;
    }

    return 0;
}

int cameraSetTargets(Camera *pCamera, Entity *pTarget1, Entity *pTarget2) {
    if (pCamera == NULL) { return IS_NULL; }

    pCamera->pTarget1 = pTarget1;
    pCamera->pTarget2 = pTarget2;
    return 0;
}

int cameraSetMode(Camera *pCamera, int newMode) {
    if (pCamera == NULL) { return IS_NULL; }

    switch (newMode) {
        case SCALING:
        case FIXED:
            pCamera->tracker.offsetX = 0;
            pCamera->tracker.offsetY = 0;
            pCamera->tracker.followX = false;
            pCamera->tracker.followY = false;
            break;
        case TRACKING_T1:
        case TRACKING_T2:
            pCamera->tracker.offsetX = CAMERA_OFFSET_X;
            pCamera->tracker.offsetY = CAMERA_OFFSET_Y;
            pCamera->tracker.followX = false;
            pCamera->tracker.followY = false;
            break;
    }

    pCamera->mode = newMode;
    
    return 0;
}

int cameraSetZoom(Camera *pCamera, float zoomScale) {
    if (pCamera == NULL) { return IS_NULL; }

    if (zoomScale > MAX_ZOOM_IN) { zoomScale = MAX_ZOOM_IN; }
    else if (zoomScale < MAX_ZOOM_OUT) { zoomScale = MAX_ZOOM_OUT; }

    pCamera->logicalWidth = round(pCamera->display.width / zoomScale);
    pCamera->logicalHeight = round(pCamera->display.height / zoomScale);
    pCamera->currentZoom = zoomScale;
    SDL_RenderSetLogicalSize(pCamera->pRenderer, pCamera->logicalWidth, pCamera->logicalHeight);
    return 0;
}

void cameraScaleToTargets(Camera *pCamera) {
    if (pCamera == NULL) { return; }

    Vec2 position1 = entityGetMidPoint(pCamera->pTarget1);
    Vec2 position2 = entityGetMidPoint(pCamera->pTarget2);
    Vec2 middlePoint;
    vectorMidPoint(&middlePoint, position1, position2);

    pCamera->position.x = middlePoint.x;
    pCamera->position.y = middlePoint.y;

    Vec2 difference;
    vectorSub(&difference, position1, position2);

    float zoomX = pCamera->display.width/(fabsf(difference.x)*1.5f*pCamera->display.globalScale);
    float zoomY = pCamera->display.height/(fabsf(difference.y)*1.5f*pCamera->display.globalScale);
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

    if (pCamera->position.x == referencePosition.x && pCamera->position.y == referencePosition.y) { 
        pCamera->velocity.x = 0.0f;
        pCamera->velocity.y = 0.0f;
        pCamera->tracker.timer = TRACKING_TIMER;
        pCamera->tracker.followY = false;
        return;
    }

    if (fabsf(pCamera->position.x-referencePosition.x) < 1.0f && fabsf(pCamera->position.y-referencePosition.y) < 1.0f) {
        pCamera->position.x = referencePosition.x;
        pCamera->position.y = referencePosition.y;
        pCamera->tracker.followY = false;
        return;
    }

    if (pCamera->tracker.timer > 0) {
        vectorAdd(&pCamera->position, pCamera->position, pCamera->velocity);
        pCamera->tracker.timer--;

        if (pCamera->tracker.timer == 0) {
            Vec2 distance;
            vectorSub(&distance, referencePosition, pCamera->position);
            pCamera->velocity.x = distance.x;
            pCamera->velocity.y = 0.0f;
            if (distance.y > 0.0f) {
                pCamera->velocity.y = distance.y*pCamera->display.aspectRatio*2;
                pCamera->tracker.followY = false;
            }
            else if (pCamera->tracker.followY) {
                pCamera->velocity.y = distance.y*0.5f;
            }
            else if (distance.y < -CAMERA_OFFSET_Y) {
                pCamera->velocity.y = distance.y*0.5f;
                pCamera->tracker.followY = true;
            }

            float scalar = 1.0f/(TRACKING_TIMER*TRACKING_MODIFIER);
            vectorScale(&pCamera->velocity, scalar);
            pCamera->tracker.timer = TRACKING_TIMER;
        }
    }
    
    return;
}

int cameraUpdate(Camera *pCamera) {
    if (pCamera == NULL) { return IS_NULL; }

    if (pCamera->pRenderer == NULL) {
        printf("Error: Camera does not have a SDL_Renderer*\n");
        return MISSING_RENDERER;
    }

    switch (pCamera->mode) {
        case SCALING:
            if (pCamera->pTarget1 == NULL && pCamera->pTarget2 == NULL) { return MISSING_TARGETS; }
            cameraScaleToTargets(pCamera);
            break;
        case TRACKING_T1:
            if (pCamera->pTarget1 == NULL) { return MISSING_TARGET1; }
            cameraTrackTarget(pCamera, entityGetMidPoint(pCamera->pTarget1));
            break;
        case TRACKING_T2:
            if (pCamera->pTarget2 == NULL) { return MISSING_TARGET2; }
            cameraTrackTarget(pCamera, entityGetMidPoint(pCamera->pTarget2));
            break;
        case FIXED:
            break;
    }
    
    return 0;
}

bool entityIsVisible(Camera const *pCamera, SDL_FRect const entity) {
    bool isVisible = true;
    float offsetY = pCamera->tracker.offsetY;
    float deltaDistanceX = fabsf(pCamera->position.x-entity.x);
    float deltaDistanceY = fabsf((pCamera->position.y-offsetY)-entity.y);

    if (deltaDistanceX > pCamera->logicalWidth*0.5f+entity.w) {
        isVisible = false;
    }
    if (deltaDistanceY > pCamera->logicalHeight*0.5f+entity.h) {
        isVisible = false;
    }

    return isVisible;
}

void adjustToCamera(Camera const *pCamera, SDL_FRect *pDst, Vec2 *pVector) {
    float globalScale = pCamera->display.globalScale;
    float offsetWidth = pCamera->logicalWidth*0.5f;
    float offsetHeight = pCamera->logicalHeight*0.5f;
    float offsetY = pCamera->tracker.offsetY;
    Vec2 cameraPosition = pCamera->position;

    if (pDst != NULL) {
        pDst->x = (pDst->x - cameraPosition.x)*globalScale + offsetWidth;
        pDst->y = (pDst->y - cameraPosition.y+offsetY)*globalScale + offsetHeight;
        pDst->w *= globalScale;
        pDst->h *= globalScale;
    }

    if (pVector != NULL) {
        pVector->x = (pVector->x - cameraPosition.x)*globalScale + offsetWidth;
        pVector->y = (pVector->y - cameraPosition.y+offsetY)*globalScale + offsetHeight;
    }

    return;
}

Vec2 cameraGetMousePosition(Camera const *pCamera) {
    Vec2 mousePosition = createVector(0.0f, 0.0f);
    int x, y;
    SDL_GetMouseState(&x, &y);
    float cameraOffsetX = ((float)x - (float)pCamera->display.width*0.5f)/(pCamera->currentZoom * pCamera->display.globalScale);
    float cameraOffsetY = ((float)y - (float)pCamera->display.height*0.5f)/(pCamera->currentZoom * pCamera->display.globalScale);
    float offsetY = pCamera->tracker.offsetY;
    mousePosition.x = pCamera->position.x + cameraOffsetX;
    mousePosition.y = pCamera->position.y-offsetY + cameraOffsetY;
    return mousePosition;
}

float cameraGetGlobalScale(Camera const *pCamera) {
    if (pCamera == NULL) { return IS_NULL; }

    return pCamera->display.globalScale;
}

int cameraGetWidth(Camera const *pCamera) {
    if (pCamera == NULL) { return IS_NULL; }

    return pCamera->logicalWidth;
}

int cameraGetHeight(Camera const *pCamera) {
    if (pCamera == NULL) { return IS_NULL; }

    return pCamera->logicalHeight;
}

Vec2 cameraGetPosition(Camera const *pCamera) {
    if (pCamera == NULL) { return createVector(0.0f, 0.0f); }

    Vec2 position = pCamera->position;
    position.y -= pCamera->tracker.offsetY;

    return position;
}

int cameraGetMode(Camera const *pCamera) {
    if (pCamera == NULL) { return IS_NULL; }

    return pCamera->mode;
}

void destroyCamera(Camera *pCamera) {
    if (pCamera == NULL) { return; }

    free(pCamera);
    return;
}