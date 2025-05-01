#pragma once 

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <jansson.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>

#include "levelHandling.h"

#define MAX_LAYERS 2


typedef struct tileset Tileset;
typedef struct clientMap ClientMap;
typedef struct serverMap ServerMap;


Tileset *createTileset(SDL_Renderer *pRenderer);

ClientMap *createClientMap(SDL_Renderer *pRenderer);

ServerMap *createServerMap();


void loadTileset(const char *filename, Tileset *devTiles);

void loadClientMap(const char *path, ClientMap *pMap);

void loadServerMap(const char *path, ServerMap *pMap);


void setTileSheetPosition(ClientMap *pMap, int x, int y);

SDL_Texture *getTileTextures(Tileset *pTileset);

xmlChar *getColumns(Tileset *pTileset);

int getMapWidth(ClientMap *pMap);

Tileset *getTileset(ClientMap *pMap);

size_t getLayerSize(ClientMap *pMap, int index);

int getLayerData(ClientMap *pMap, int layer, int index);

SDL_Rect getTileSheetPosition(ClientMap *pMap);

//---------------Server-------------------------

int getMapWidth_Server(ServerMap *pMap);

size_t getLayerSize_Server(ServerMap *pMap, int layer);

int getLayerData_Server(ServerMap *pMap, int layer, int index);

void destroyMap_Server(ServerMap *pMap);
