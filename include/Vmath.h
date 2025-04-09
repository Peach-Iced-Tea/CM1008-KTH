#pragma once
#include <math.h>
#include <stdlib.h>

typedef struct vec2 Vec2;


Vec2 *createVector();

void initVec(Vec2 *vector, float p_x, float p_y);

void vectorAdd(Vec2 *vectorOut, Vec2 *vector1, Vec2 *vector2);

void vectorSub(Vec2 *vectorOut, Vec2 *vector1, Vec2 *vector2);

float vectorMulti(Vec2 *vectorOut, Vec2 *vector1, Vec2 *vector2);

void vectorScale(Vec2 *vectorOut, float scalar);

float vectorLength(Vec2 *vector);

void vectorAccel(Vec2 *vector, float a_x, float a_y);

float vectorGetX(Vec2 *vector);

float vectorGetY(Vec2 *vector);

void printVector(Vec2 *vector);

