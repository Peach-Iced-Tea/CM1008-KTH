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


void mapLoadTileset(const char *filename, Tileset *devTiles);

void mapLoadClient(const char *path, ClientMap *pMap);

void mapLoadServer(const char *path, ServerMap *pMap);


void mapSetTileSheetPosition(ClientMap *pMap, int x, int y);

SDL_Texture *mapGetTileTextures(Tileset *pTileset);

xmlChar *mapGetColumns(Tileset *pTileset);

int mapGetWidth(ClientMap *pMap);

Tileset *mapGetTileset(ClientMap *pMap);

size_t mapGetLayerSize(ClientMap *pMap, int index);

int mapGetLayerData(ClientMap *pMap, int layer, int index);

SDL_Rect mapGetTileSheetPosition(ClientMap *pMap);

void destroyMap(ClientMap *pMap);

//---------------Server-------------------------

int mapGetWidth_Server(ServerMap *pMap);

size_t mapGetLayerSize_Server(ServerMap *pMap, int layer);

int mapGetLayerData_Server(ServerMap *pMap, int layer, int index);

void destroyMap_Server(ServerMap *pMap);
