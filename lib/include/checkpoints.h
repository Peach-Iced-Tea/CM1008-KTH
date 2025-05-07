#pragma once
#include "vmath.h"
#include "player.h"

typedef struct checkpoint Checkpoint;

void gotoCheckpoint(Player *pPlayer);
void setNewCheckpoint(Player *pPlayer, Checkpoint *newCheckpoint);