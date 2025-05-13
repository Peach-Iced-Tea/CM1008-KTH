#pragma once 
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <jansson.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>

//#include "levelHandling.h"

#define MAX_LAYERS 3


typedef struct tileset Tileset;
typedef struct clientMap ClientMap;
typedef struct serverMap ServerMap;


Tileset *createTileset(SDL_Renderer *renderer);

ClientMap *createClientMap(SDL_Renderer *renderer);

ServerMap *createServerMap();


void mapLoadTileset(const char *filename, Tileset *devTiles);

void mapLoadClient(const char *path, ClientMap *map);

void mapLoadServer(const char *path, ServerMap *map);


void mapSetTileSheetPosition(ClientMap *map, int x, int y);

SDL_Texture *mapGetTileTextures(Tileset *tileset);

xmlChar *mapGetColumns(Tileset *tileset);

int mapGetWidth(ClientMap *map);

Tileset *mapGetTileset(ClientMap *map);

size_t mapGetLayerSize(ClientMap *map, int index);

int mapGetLayerData(ClientMap *map, int layer, int index);

SDL_Rect mapGetTileSheetPosition(ClientMap *map);

void destroyMap(ClientMap *map);

//---------------Server-------------------------

int mapGetWidth_Server(ServerMap *map);

size_t mapGetLayerSize_Server(ServerMap *map, int layer);

int mapGetLayerData_Server(ServerMap *map, int layer, int index);

void destroyMap_Server(ServerMap *map);
