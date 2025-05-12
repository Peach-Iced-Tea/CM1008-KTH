#pragma once 

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <jansson.h>


#define MAX_BLOCKS 1000
#define TILE_SIZE 32

struct Game;

typedef enum {
    STATIC,
    HORIZONTAL,
    VERTICAL,
    DROP,
    LIFT,
    SWITCH
} BlockType;

typedef struct {
    SDL_Texture* texture;
    char* filePath;
    int width;
    int height;
} MapSheetImage;

typedef struct {
    int* data;
    int width;
    int height;
} MapLayer;

typedef struct {
    int blockId;
    float x, y;
    int origX, origY;
    BlockType type;
    int direction;
    float speed;
    float moveRange;
    int active;
    int groupId;
} Block;

typedef struct {
    Block* blocks;
    MapLayer layerNr1_tile;
    MapSheetImage mapSheetImage;
    int blockCount;
} Map;

void loadMap(struct Game *pGame);

Map* createMap_Server(const char* filePath);

void destroyMap(Map* pMap);



