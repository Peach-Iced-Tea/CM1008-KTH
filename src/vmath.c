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

float vectorLength(Vec2 const vector1, Vec2 const vector2) {
    float x = powf(vector1.x-vector2.x, 2);
    float y = powf(vector1.y-vector2.y, 2);
    return sqrtf(x+y);
}

void vectorMidPoint(Vec2 *vectorResult, Vec2 const vector1, Vec2 const vector2) {
    vectorResult->x = (vector1.x + vector2.x)*0.5f;
    vectorResult->y = (vector1.y + vector2.y)*0.5f;
    return;
}

float vectorGetAngle(Vec2 const vector1, Vec2 const vector2) {
    float dx = (vector2.x - vector1.x);
    float dy = (vector2.y - vector1.y);


    //2nd quadrant
    if(dx < 0 && dy > 0) {
        return M_PI + atanf(dy/dx);
    }
    //3rd quadrant
    if(dx < 0 && dy < 0) {
        return atanf(dy/dx) + M_PI;
    }
    //4th quadrant
    if(dx > 0 && dy < 0) {
        return 2 * M_PI + atanf(dy/dx);
    }
    //1st quadrant
    return atanf(dy/dx);
   
}
