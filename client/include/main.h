#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <stdbool.h>

#include "vmath.h"
#include "physics.h"
#include "entity.h"
#include "input.h"
#include "camera.h"
#include "renderWindow.h" 
#include "utils.h"
#include "player.h"
#include "networkData.h"
#include "client.h"
#include "menu.h"
#include "tongue.h"
#include "mapParser.h"

typedef struct Game {   // koden kunde inte läsa om jag inte defnerar här igen Game
    RenderWindow *pWindow;
    Camera *pCamera;
    Player *players[MAX_PLAYERS];
    Input *pInput;
    Menu *pMenu;
    Client *pClient;  
    Map *pMap;   // bytt ut Clientmap mot map
    DynamicArray *pHitforms;
} Game;


/////////////////////////////////////////////////////////////////////////////////////////////////////////

// Prince
// Ändrat hur vi läser map data så att vi inte berhöver libxml längre utan räcker det med json.
//      Hämtar data och sedan lägger vi block betjande med hjälp av samma fil i detta fal var block värden i Object_Folder
// 
// Har lag till funktionen updatePlatforms som uppdaterar block/platform/entity pos beroande av deras typ
// 
// 
// 
//
//