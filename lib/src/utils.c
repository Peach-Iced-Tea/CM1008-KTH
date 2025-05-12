#include "utils.h"

struct dynamicArray {
    void **objects;
    size_t size;
    size_t capacity;
    int arrayType;
};

DynamicArray *createDynamicArray(int arrayType) {
    DynamicArray *pArray = malloc(sizeof(DynamicArray));
    switch (arrayType) {
        case ARRAY_ENTITIES:
            pArray->objects = malloc(4 * sizeof(Entity*));
            break;
        case ARRAY_HITBOXES:
            pArray->objects = malloc(4 * sizeof(Hitbox*));
            break;
    }

    if (!pArray->objects) {
        printf("Error failed allocation of memory for array\n");
        return NULL;
    }

    pArray->size = 0;
    pArray->capacity = 4;
    pArray->arrayType = arrayType;
    return pArray;
}

int arrayAddObject(DynamicArray *pArray, void *pNewObject) {
    if (pArray == NULL) { return 2; }

    if (pArray->size == pArray->capacity) {
        pArray->capacity *=2;
        switch (pArray->arrayType) {
            case ARRAY_ENTITIES:
                pArray->objects = realloc(pArray->objects, pArray->capacity * sizeof(Entity*));
                break;
            case ARRAY_HITBOXES:
                pArray->objects = realloc(pArray->objects, pArray->capacity * sizeof(Hitbox*));
                break;
        }

        if (!pArray->objects) {
            printf("Error failed to reallocate memory for array\n");
            return 1;
        }
    }

    pArray->objects[pArray->size++] = pNewObject;
    return 0;
}

void *arrayGetObject(DynamicArray *pArray, int index) {
    return pArray->objects[index];
}

size_t arrayGetSize(DynamicArray *pArray) {
    return pArray->size;
}

void destroyDynamicArray(DynamicArray *pArray) {
    if (pArray == NULL) { return; }

    for (size_t i = 0; i < pArray->size; i++) {
        switch (pArray->arrayType) {
            case ARRAY_ENTITIES:
                destroyEntity(pArray->objects[i]);
                break;
            case ARRAY_HITBOXES:
                destroyHitbox(pArray->objects[i]);
                break;
        }
    }

    free(pArray->objects);
    free(pArray);
    return;
}