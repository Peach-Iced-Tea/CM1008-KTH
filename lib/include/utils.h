#pragma once
#include "entity.h"

typedef enum ArrayType {
    ARRAY_ENTITIES, ARRAY_HITBOXES
} ArrayType;

typedef struct dynamicArray DynamicArray;

DynamicArray *createDynamicArray(int arrayType);

int arrayAddObject(DynamicArray *array, void *newObject);

void *arrayGetObject(DynamicArray *array, int index);

size_t arrayGetSize(DynamicArray *array);

void destroyDynamicArray(DynamicArray *array);