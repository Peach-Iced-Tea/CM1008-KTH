
#include "animation.h"


void animationUpdateRunning(SDL_Rect *pAnimation, int *pTimer) {
    pAnimation->x = 128;
    if ((*pTimer) == 6) {
        pAnimation->y += 32;
        if (pAnimation->y > 160) { pAnimation->y = 0;}
        (*pTimer) = 0;
    }
    (*pTimer)++;
}

bool animationUpdateRotation(SDL_Rect *pAnimation, float angle) {
    pAnimation->x = 64; 
    bool tmp = false;

 

    if ((angle > ((2*M_PI) - (M_PI/36.0f)) && angle < 0.0f) || (angle > 0.0f && angle < (M_PI/12.0f))) {
        pAnimation->y = 32;   
    }
    else if (angle > (M_PI - (M_PI/12.0f)) && angle < M_PI + (M_PI/36.0f)) {
        pAnimation->y = 32;
        tmp = true;
    }
    else if (angle > (M_PI/12.0f) && angle < (M_PI/3.0f)) {
        pAnimation->y = 64;
    }
    else if (angle > (2*(M_PI)/3.0f) && angle < (M_PI - (M_PI/12.0f))) {
        pAnimation->y = 64;
        tmp = true;
    }
    else if (angle > (M_PI/3.0f) && angle < ((2*M_PI)/3.0f)) {
        pAnimation->y = 96;
    }
    else if (angle > ((2*M_PI) - (M_PI/4.0f)) && angle < ((2*M_PI) - (M_PI/36.0f))) {
        pAnimation->y = 0;
    }
    else if (angle > (M_PI + (M_PI/36.0f)) && angle < (M_PI + (M_PI/4.0f))) {
        pAnimation->y = 0;
        tmp = true;
    }

    else if (angle > (M_PI + (M_PI/4.0f)) && angle < ((2*M_PI) - (M_PI/4.0f))) {
        pAnimation->y = 128;
    }

    return tmp;
}