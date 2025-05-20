#pragma once
#include <stdbool.h>

#include "vmath.h"
#include "entity.h"
#include "input.h"
#include "tongue.h"
#include "animation.h"

#define PLAYER_1 0
#define PLAYER_2 1

#define PLAYER_START_X 1088.0f
#define PLAYER_START_Y 3968.0f

typedef enum PlayerState {
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
Player *createPlayer(Vec2 position, int id);

/*
Handles key inputs related to the handling of the Player data type.

Such as moving the given Player left and right, as well as jumping.
*/
void playerHandleInput(Player *player, Input const *input);

/*
Update the current shown frame of the player based on the internal state of the given Player.
*/
void playerUpdateAnimation(Player *player);

/*
Update the given Player based on what PlayerState it is in currently.

This will update things such as current acceleration applied by gravity
and the current sprite that is shown.
*/
void playerUpdateState(Player *player);

/*
Updates the given Player's positional values based on which PlayerState it is currently in.

Unless it is in PlayerState 'ROTATING', in which case it will return a Vec2 containing the position of the object being rotated.
*/
void playerUpdatePosition(Player *player, float deltaTime);

/*
Determines how the player should move the object that it has grabbed based on the given target angle.
*/
void playerCalculateRotation(Player *player, float targetAngle);

/*
Check whether the given Player has collided with the given Entity.

Returns an int value based on where the Player is in relation to the Entity (returns 0 if no collision was detected).
*/
int playerCheckCollision(Player *pPlayer, Hitbox *pObject, bool applyCollisionResponse);

/*
Calculates the angle between the player's middle point and the given reference position.
*/
void playerCalculateAngle(Player *pPlayer, Vec2 referencePosition);

/*
Sets the position of the Player and all objects related to it with the values inside newPosition.
*/
void playerSetPosition(Player *player, Vec2 newPosition);

/*
Override the current PlayerState of the given Player with a new state.

Some PlayerStates will not allow you to override them, which will result in the function returning 'false',
otherwise the function will return 'true'.
*/
bool playerSetState(Player *player, int newState);

/*
Set the current sheet position of the given Player.
This should most likely only be called when the server wants to override info.
*/
bool playerSetSheetPosition(Player *player, Vec2 const newPosition);

/*
If the Player has hit an object with its tongue then the object should be passed into this function.

Only *entity OR *teammate should be filled at a time otherwise the function exits immediately.
*/
void playerSetGrabbedEntity(Player *player, Entity *entity, Player *teammate);

/*
Returns data for the given Player, such as position values and velocity.
*/
PlayerInfo playerGetInfo(Player const *player);

/*
Get the Entity representing the given Player's body.
*/
Entity playerGetBody(Player const *player);

/*
Get the Hitbox of the body (Entity) of the given Player.
*/
Hitbox *playerGetBodyHitbox(Player const *player);

/*
Get the middle point of the given Player.
*/
Vec2 playerGetMidPoint(Player const *player);

/*
Get the Tongue of the given Player.
*/
Tongue *playerGetTongue(Player const *player);

/*
This function is very aggressive and moves the given Player with no checks.

!!! Should only be used if you're completely sure.
*/
void playerOverrideMove(Player *player, Vec2 velocity);

/*
This function is very aggressive and overrides the PlayerState of the given Player with no checks.

!!! Should only be used if you're completely sure.
*/
void playerOverrideState(Player *player, PlayerState newState);

/*
This function is very aggressive and overrides the velocities of the given Player with no checks.

!!! Should only be used if you're completely sure.
*/
void playerOverrideVelocity(Player *player, Vec2 newVelocity, float newRotation);

/*
Use this function to destroy the given Player pointer and free up memory.
*/
void destroyPlayer(Player *player);