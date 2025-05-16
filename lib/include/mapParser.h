#pragma once 
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <jansson.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>

#include "vmath.h"
//#include "levelHandling.h"

#define MAX_LAYERS 3


typedef struct tileset Tileset;
typedef struct map Map;


Tileset *createTileset(SDL_Renderer *renderer);

Map *createMap(SDL_Renderer *renderer);

void mapLoadTileset(Tileset *devTiles, const char *filename);

void mapLoadDataFromFile(Map *map, const char *path);


void mapSetTileSheetPosition(Map *map, int x, int y);

SDL_Texture *mapGetTileTextures(Tileset *tileset);

xmlChar *mapGetColumns(Tileset *tileset);

int mapGetWidth(Map *map);

Tileset *mapGetTileset(Map *map);

size_t mapGetLayerSize(Map *map, int index);

int mapGetLayerData(Map *map, int layer, int index);

SDL_Rect mapGetTileSheetPosition(Map *map);

Vec2 mapGetSize(Map const *map);

void destroyMap(Map *map);
