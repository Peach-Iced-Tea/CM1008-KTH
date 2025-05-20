#pragma once
#include <stdio.h>
#include <jansson.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>

#include "vmath.h"
#include "entity.h"

#define MAX_LAYERS 3

typedef enum MapIndex {
    MAP_DEMO
} MapIndex;

typedef enum LayerType {
    LAYER_HITBOXES, LAYER_TILE_TEXTURES, LAYER_OBSTACLES
} LayerType;

typedef struct tileset Tileset;
typedef struct map Map;

Map *createMap();

/*
Loads data from the given MapIndex, this includes things such as information
about hitboxes and objects that are present in the world.
*/
int mapLoadDataFromFile(Map *map, MapIndex index);

/*
Returns the full width of the given Map in tile count.
*/
int mapGetWidth(Map const *map);

/*
Returns the path to the map folder used by the current given Map.
*/
char const *mapGetFolderPath(Map const *map);

/*
Returns the path to the tileset spritesheet used by the current given Map.
*/
char const *mapGetTilesetPath(Map const *map);

/*
Returns the amount of layers that the map has loaded from the file.
*/
int mapGetLayerCount(Map const *map);

/*
Returns the size of the given layer.
*/
size_t mapGetLayerSize(Map const *map, int index);

/*
Gathers information about the object at the given index on the specified layer.

Also fills up the given Vec2 with the absolute position of the object.
*/
int mapGetLayerData(Map const *map, int layer, int index, Vec2 *position);

/*
Returns the full size of the map in pixel count.
*/
Vec2 mapGetSize(Map const *map);

/*
Returns the standardized width of all the tiles in the map. 
*/
int mapGetTileWidth(Map const *map);

/*
Returns the standardized height of all the tiles in the map. 
*/
int mapGetTileHeight(Map const *map);

/*
Fills the given Entity with information about the tile at the given index.

Information such as absolute position in the world, width and height,
and also where the texture is located in the spritesheet
*/
bool mapGetTileInfo(Map const *map, int index, Entity *tileToFill);

/*
Use this function to destroy the given Map pointer and free up memory.
*/
void destroyMap(Map *map);
