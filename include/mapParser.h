#pragma once 


#include <SDL2/SDL.h>
#include <stdio.h>
#include <jansson.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>


typedef struct tileset Tileset;
typedef struct map Map;


Tileset *createTileset();

void parseTileset(const char *filename, Tileset *devTiles);

Tileset *createTileset();

Map *createMap();

void loadTileset(const char *filename, Tileset *devTiles);

void loadMap(const char *path, Map *pMap);

Tileset *getTileset(Map *pMap);


