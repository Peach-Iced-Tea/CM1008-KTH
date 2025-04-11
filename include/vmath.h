#pragma once
#include <math.h>
#include <stdlib.h>

/*
2-dimensional vector, containing two float values.
*/
typedef struct {
    float x, y;
} Vec2;

// Create a vector using two float values, returns a Vec2* pointer.
Vec2 createVector(float x, float y);

// Add vector2 to vector1, returns the result inside the first parameter.
void vectorAdd(Vec2 *vectorResult, Vec2 const vector1, Vec2 const vector2);

// Subtract vector2 from vector1, returns the result inside the first parameter.
void vectorSub(Vec2 *vectorResult, Vec2 const vector1, Vec2 const vector2);

// Multiplies two vectors with each other and returns the result.
float vectorMulti(Vec2 const vector1, Vec2 const vector2);

// Multiplies the two float values of the given vector with a scalar.
void vectorScale(Vec2 *vector, float scalar);

// Get the length/magnitude of the given vector.
float vectorLength(Vec2 const vector);

//void vectorMin(Vec2 *vectorResult, Vec2 const vector1, Vec2 const vector2);

//void vectorMax(Vec2 *vectorResult, Vec2 const vector1, Vec2 const vector2);

void printVector(Vec2 const *vector);

