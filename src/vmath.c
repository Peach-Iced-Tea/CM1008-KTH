#include <stdio.h>

#include "Vmath.h"


/* struct vector2f{
    float x, y;
}; */

void initVecZero(Vector2f *vector)
{
    vector->x = 0.0f;
    vector->y = 0.0f;
}

void initVec(Vector2f *vector, float p_x, float p_y)
{
    vector->x = p_x;
    vector->y = p_y;
}

void printVector(Vector2f *vector)
{
    printf("%f, %f\n", vector->x, vector->y);
}