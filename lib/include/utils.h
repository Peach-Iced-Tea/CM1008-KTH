#pragma once
#include "entity.h"
#include "checkpoint.h"
#include "obstacle.h"

typedef enum ArrayType {
    ARRAY_ENTITIES, ARRAY_HITBOXES, ARRAY_CHECKPOINTS, ARRAY_OBSTACLES
} ArrayType;

typedef struct dynamicArray DynamicArray;

DynamicArray *createDynamicArray(ArrayType arrayType);

/*
Add an object to the DynamicArray, anything can be put into the array.

Be mindful to keep track of what is put into the array.
*/
int arrayAddObject(DynamicArray *array, void *newObject);

/*
Returns the object located at the position of index.

The object is returned as a void*, it's up to you to keep track of what the object is.
Using arrayGetType() is a good idea to make sure you know what is expected to be returned.
*/
void *arrayGetObject(DynamicArray *array, int index);

/*
Returns the current size of the given DynamicArray.

The size is the amount of objects that have been stored inside it.
*/
size_t arrayGetSize(DynamicArray const *array);

/*
Returns the type of the given DynamicArray.
*/
ArrayType arrayGetType(DynamicArray const *array);

/*
Use this function to destroy the given DynamicArray pointer and free up memory.
*/
void destroyDynamicArray(DynamicArray *array);