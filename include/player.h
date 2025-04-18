#pragma once
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "entity.h"
#include "vmath.h"

typedef enum {
    IDLE, RUNNING, JUMPING, FALLING, ROTATING, FLYING
} PlayerState;

typedef struct player Player;

/*
Create a player with a given SDL_Texture* and positional values for where it should be in the world.
*/
Player *createPlayer(Vec2 position, SDL_Texture *pTexture);

/*
This function will handle keyboard/mouse inputs related to the player, such as movement or mouse clicks.
*/
bool playerHandleInput(Player *pPlayer);

/*
Update the given Player based on what PlayerState it is in currently.

This will update things such as current acceleration applied by gravity
and the current sprite that is shown.

Call this function inside a timestep controlled if-statement.
*/
void playerUpdateState(Player *pPlayer, float deltaTime);

/*
Updates the given Player's positional values based on which PlayerState it is currently in.

Unless it is in PlayerState 'ROTATING', in which case it will return a Vec2 containing the position of the object being rotated.
*/
Vec2 playerUpdatePosition(Player *pPlayer, float deltaTime);

/*
Check whether the given Player has collided with the given Entity.

Returns an int value based on where the Player is in relation to the Entity (returns 0 if no collision was detected).
*/
int playerCheckCollision(Player *pPlayer, Entity *pEntity);

/*
Override the current PlayerState of the given Player with a new state.

Some PlayerStates will not allow you to override them, which will result in the function returning 'false',
otherwise the function will return 'true'.
*/
bool playerSetState(Player *pPlayer, int newState);

/*
Set the radius from the center of the given Player to a point in space.

The point in space could be the midpoint of another Player, an Entity or a the position of the mouse.
*/
bool playerSetRadius(Player *pPlayer, float radius);

/*
Set the angle between the center of the given Player and a point in space.

The point in space could be the midpoint of another Player, an Entity or a the position of the mouse.
*/
void playerSetReferenceAngle(Player *pPlayer, float newAngle);

/*
Get the current PlayerState of the given Player.
*/
int playerGetState(Player *pPlayer);

/*
Get the current Entity representing the given Player's body.
*/
Entity *playerGetBody(Player const *pPlayer);

/*
Get the current sprite sheet position of the given Player.
*/
SDL_Rect playerGetSheetPosition(Player const *pPlayer);

/*
Get whether the given Player is currently pressing down on the left mouse button.
*/
bool playerGetMouseClick(Player const *pPlayer);

/*
Use this function to destroy the given Player pointer and free up memory.
*/
void destroyPlayer(Player *pPlayer);