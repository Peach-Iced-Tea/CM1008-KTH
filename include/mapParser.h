#pragma once 


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <jansson.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>


typedef struct tileset Tileset;

typedef struct map Map;

Tileset *createTileset(SDL_Renderer *pRenderer);

void parseTileset(const char *filename, Tileset *devTiles);

Map *createMap(SDL_Renderer *pRenderer);

void loadTileset(const char *filename, Tileset *devTiles);

void loadMap(const char *path, Map *pMap);

void setTileSheetPosition(Map *pMap, int x, int y);

SDL_Texture *getTileTextures(Tileset *pTileset);

xmlChar *getColumns(Tileset *pTileset);

int getMapWidth(Map *pMap);

Tileset *getTileset(Map *pMap);

size_t getLayerSize(Map *pMap, int index);

int getLayerData(Map *pMap, int layer, int index);

SDL_Rect getTileSheetPosition(Map *pMap);
