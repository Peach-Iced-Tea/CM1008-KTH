#pragma once
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "entity.h"
#include "vmath.h"
#include "input.h"

/*
SCALING: The camera will use two Entities as reference points and stay in the middle of them while scaling the zoom level automatically.
TRACKING_T1: The camera will track the first Entity given with cameraSetTargets(), zoom level can be manually set with cameraSetZoom().
TRACKING_T2: The camera will track the second Entity given with cameraSetTargets(), zoom level can be manually set with cameraSetZoom().
FIXED: The camera is set to a specific position and will not move unless manually done.
*/
typedef enum CameraMode {
    SCALING, TRACKING_T1, TRACKING_T2, FIXED
} CameraMode;

typedef enum CameraError {
    IS_NULL = 1, MISSING_RENDERER, MISSING_TARGETS, MISSING_TARGET1, MISSING_TARGET2
} CameraError;

typedef struct camera Camera;

#define MAX_ZOOM_IN 1.0f
#define MAX_ZOOM_OUT 1.5f
#define REFERENCE_WIDTH 736
#define REFERENCE_HEIGHT 414

/*
Create a Camera with the current resolution of the game window.
*/
Camera *createCamera(int width, int height, int refreshRate, SDL_Renderer *renderer, int cameraMode);

/*
Handles key inputs related to the functions of the Camera data type.

Such as setting what mode the given Camera should be set to.
*/
void cameraHandleInput(Camera *camera, Input const *inputs);

/*
Update the position as well as the zoom level of the given Camera.

What this does function does depends on what mode the given Camera is set to.
*/
int cameraUpdate(Camera *camera, Entity const target1, Entity const target2);

/*
Check if the given Entity is visible inside the dimensions of the given Camera.

Returns 'true' if it is inside the camera view, 'false' if it is outside the camera view.
*/
bool cameraEntityIsVisible(Camera const *camera, SDL_FRect const entity);

/*
Use this function to adjust the position of a SDL_FRect and/or a Vec2 to the given Camera.

Either *dst or *position can be set as NULL.
*/
void cameraAdjustToViewport(Camera const *camera, SDL_FRect *dst, Vec2 *position);

/*
Set the SDL_Renderer* that the given Camera should use when scaling.
*/
int cameraSetRenderer(Camera *camera, SDL_Renderer *renderer);

/*
Set what mode the given Camera should be set to.
*/
int cameraSetMode(Camera *camera, int newMode);

/*
Set the camera to a specific zoom level.

A higher zoomScale means that the Camera is closer up, 
and a lower zoomScale means that the Camera is further away.
*/
int cameraSetZoom(Camera *camera, float zoomScale);

/*
Returns the absolute position of the mouse cursor in the game world.
*/
Vec2 cameraGetMousePosition(Camera const *camera);

/*
Get the global scale used to make the rendered view consistent across resolutions.
*/
float cameraGetGlobalScale(Camera const *camera);

/*
Returns the logical width of the given Camera.

The logical dimensions are the width and height of the camera itself, not the actual resolution of the game.
*/
int cameraGetWidth(Camera const *camera);

/*
Returns the logical height of the given Camera.

The logical dimensions are the width and height of the camera itself, not the actual resolution of the game.
*/
int cameraGetHeight(Camera const *camera);

/*
Returns a Vec2 containing the position(x,y) of the given Camera.
*/
Vec2 cameraGetPosition(Camera const *camera);

/*
Returns the current mode that the given Camera is set to.
*/
CameraMode cameraGetMode(Camera const *camera);

/*
Use this function to destroy the given Camera pointer and free up memory.
*/
void destroyCamera(Camera *camera);