#pragma once


struct vector2f{
    float x, y;
}; 
typedef struct vector2f Vector2f;


void initVecZero(Vector2f *vector);

void initVec(Vector2f *vector, float p_x, float p_y);

void printVector(Vector2f *vector);

