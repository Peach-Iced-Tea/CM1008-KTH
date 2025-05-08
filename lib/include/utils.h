#pragma once
#include "entity.h"

#define ARRAY_ENTITIES 1
#define ARRAY_HITBOXES 2

typedef struct dynamicArray DynamicArray;

DynamicArray *createDynamicArray(int arrayType);

int arrayAddObject(DynamicArray *array, void *newObject);

void *arrayGetObject(DynamicArray *array, int index);

size_t arrayGetSize(DynamicArray *array);

void destroyDynamicArray(DynamicArray *array);