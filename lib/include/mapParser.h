#pragma once 
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <jansson.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>

#include "entity.h"
//#include "levelHandling.h"

#define MAX_LAYERS 3

typedef enum LayerType {
    LAYER_HITBOXES, LAYER_TILE_TEXTURES, LAYER_OBSTACLES
} LayerType;

typedef struct tileset Tileset;
typedef struct map Map;

Tileset *createTileset();

Map *createMap();

void mapLoadTileset(Tileset *devTiles, const char *filePath);

void mapLoadDataFromFile(Map *map, const char *filePath);

xmlChar *mapGetColumns(Tileset const *tileset);

int mapGetWidth(Map const *map);

Tileset *mapGetTileset(Map const *map);

size_t mapGetLayerSize(Map const *map, int index);

int mapGetLayerData(Map const *map, int layer, int index);

Vec2 mapGetSize(Map const *map);

bool mapGetTileInfo(Map const *map, int index, Entity *tileToFill);

void destroyMap(Map *map);
