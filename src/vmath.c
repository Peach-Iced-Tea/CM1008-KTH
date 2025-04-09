#include <stdio.h>

#include "Vmath.h"


struct vec2 {
    float x, y;
};

Vec2 *createVector() {
    Vec2 *vector = malloc(sizeof(Vec2));
    vector->x = 0.0f;
    vector->y = 0.0f;
    return vector;
}

// Initiate a vector with specified x and y coordinates.
void initVec(Vec2 *vector, float p_x, float p_y) {
    vector->x = p_x;
    vector->y = p_y;
}

// Add vector2 to vector1.
void vectorAdd(Vec2 *vectorOut, Vec2 *vector1, Vec2 *vector2) {
    vectorOut->x = vector1->x + vector2->x;
    vectorOut->y = vector1->y + vector2->y;
}

// Subtract vector2 from vector1.
void vectorSub(Vec2 *vectorOut, Vec2 *vector1, Vec2 *vector2) {
    vectorOut->x = vector1->x - vector2->x;
    vectorOut->y = vector1->y - vector2->y;
}

// Multiply two vectors with each other.
float vectorMulti(Vec2 *vectorOut, Vec2 *vector1, Vec2 *vector2) {
    float dotProduct = 0.0f;
    dotProduct += vector1->x * vector2->x;
    dotProduct += vector1->y * vector2->y;
    return dotProduct;
}

// Multiply the vector with a scalar.
void vectorScale(Vec2 *vectorOut, float scalar) {
    vectorOut->x *= scalar;
    vectorOut->y *= scalar;
}

// Get the length of the magnitude/length.
float vectorLength(Vec2 *vector) {
    float x = pow(vector->x, 2);
    float y = pow(vector->y, 2);
    return sqrtf(x+y);
}

void vectorAccel(Vec2 *vector, float a_x, float a_y) {
    vector->x *= a_x;
    vector->x *= a_y;
}

// Get the x coordinate of a vector.
float vectorGetX(Vec2 *vector) {
    return vector->x;
}

// Get the y coordinate of a vector.
float vectorGetY(Vec2 *vector) {
    return vector->y;
}

void printVector(Vec2 *vector) {
    printf("%f, %f\n", vector->x, vector->y);
}