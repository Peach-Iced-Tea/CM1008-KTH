#pragma once
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "entity.h"
#include "vmath.h"
#include "input.h"
#include "tongue.h"

#define PLAYER_1 0
#define PLAYER_2 1

#define PLAYER_START_X 48.0f
#define PLAYER_START_Y 48.0f

typedef enum {
    IDLE, RUNNING, JUMPING, FALLING, ROTATING, FLYING, SHOOTING, RELEASE, LOCKED
} PlayerState;

typedef struct {
    Vec2 position;
    Vec2 velocity;
    SDL_Rect sheetPosition;
    float rotateVelocity;
    float tongueAngle;
    PlayerState state;
} PlayerInfo;

typedef struct player Player;

/*
Create a player with a given SDL_Texture* and positional values for where it should be in the world.
*/
Player *createPlayer(Vec2 position, SDL_Renderer *pRenderer, int id);

/*
Handles key inputs related to the handling of the Player data type.

Such as moving the given Player left and right, as well as jumping.
*/
void playerHandleInput(Player *pPlayer, Input const *pInputs);

/*

*/
void playerUpdateAnimation(Player *pPlayer);

/*
Update the given Player based on what PlayerState it is in currently.

This will update things such as current acceleration applied by gravity
and the current sprite that is shown.
*/
void playerUpdateState(Player *pPlayer);

/*
Updates the given Player's positional values based on which PlayerState it is currently in.

Unless it is in PlayerState 'ROTATING', in which case it will return a Vec2 containing the position of the object being rotated.
*/
void playerUpdatePosition(Player *pPlayer, float deltaTime);

/*

*/
void playerCalculateRotation(Player *pPlayer, float targetAngle);

/*
Check whether the given Player has collided with the given Entity.

Returns an int value based on where the Player is in relation to the Entity (returns 0 if no collision was detected).
*/
int playerCheckCollision(Player *pPlayer, Hitbox *pObject);

/*

*/
void playerSetPosition(Player *pPlayer, Vec2 newPosition);

/*
Override the current PlayerState of the given Player with a new state.

Some PlayerStates will not allow you to override them, which will result in the function returning 'false',
otherwise the function will return 'true'.
*/
bool playerSetState(Player *pPlayer, int newState);

/*
Set the current sheet position of the given Player.
This should most likely only be called when the server wants to override info.
*/
bool playerSetSheetPosition(Player *pPlayer, Vec2 const newPosition);

/*

*/
void playerSetGrabbedEntity(Player *pPlayer, Entity *pEntity);

/*
Returns data for the given Player, such as position values and velocity.
*/
PlayerInfo playerGetInfo(Player const *pPlayer);

/*
Get the current Entity representing the given Player's body.
*/
Entity *playerGetBody(Player const *pPlayer);

/*

*/
Tongue *playerGetTongue(Player const *pPlayer);

/*

*/
Hitbox *playerGetBodyHitbox(Player const *pPlayer);

/*

*/
void playerOverrideState(Player *pPlayer, PlayerState newState);

/*

*/
void playerOverrideVelocity(Player *pPlayer, Vec2 newVelocity, float newRotation);

/*
Use this function to destroy the given Player pointer and free up memory.
*/
void destroyPlayer(Player *pPlayer);