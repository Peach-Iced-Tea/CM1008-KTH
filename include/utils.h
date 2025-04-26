#pragma once
#include <SDL2/SDL.h>

#include "entity.h"
#include "physics.h"

#define ARRAY_ENTITIES 1
#define ARRAY_HITBOXES 2

typedef struct dynamicArray DynamicArray;

DynamicArray *createDynamicArray();

int addEntity(DynamicArray *pArray, float x, float y, SDL_Texture *pTexture, int hitboxType);

int addHitbox(DynamicArray *pArray, float x, float y, int w, int h, int hitboxType);

void *arrayGetObject(DynamicArray *pArray, int index);

size_t arrayGetSize(DynamicArray *pArray);

void destroyDynamicArray(DynamicArray *pArray);