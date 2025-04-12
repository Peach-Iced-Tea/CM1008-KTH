#include "vmath.h"

Vec2 createVector(float x, float y) {
    Vec2 vector;
    vector.x = x;
    vector.y = y;
    return vector;
}

void vectorAdd(Vec2 *vectorResult, Vec2 const vector1, Vec2 const vector2) {
    vectorResult->x = vector1.x + vector2.x;
    vectorResult->y = vector1.y + vector2.y;
    return;
}

void vectorSub(Vec2 *vectorResult, Vec2 const vector1, Vec2 const vector2) {
    vectorResult->x = vector1.x - vector2.x;
    vectorResult->y = vector1.y - vector2.y;
    return;
}

float vectorMulti(Vec2 const vector1, Vec2 const vector2) {
    float dotProduct = 0.0f;
    dotProduct += vector1.x * vector2.x;
    dotProduct += vector1.y * vector2.y;
    return dotProduct;
}

void vectorScale(Vec2 *vector, float scalar) {
    vector->x *= scalar;
    vector->y *= scalar;
    return;
}

float vectorLength(Vec2 const vector) {
    float x = pow(vector.x, 2);
    float y = pow(vector.y, 2);
    return sqrtf(x+y);
}