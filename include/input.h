#pragma once
#include <SDL2/SDL.h>
#include <stdbool.h>

#define KEY_NULL -1
#define KEY_STATE_UP 0
#define KEY_STATE_DOWN 1
#define KEY_STATE_HOLD 20

typedef enum {
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    KEY_W, KEY_A, KEY_S, KEY_D, KEY_Q, KEY_E, KEY_R, KEY_T, KEY_F, KEY_G,
    KEY_SPACE, KEY_COMMA, KEY_PERIOD, KEY_RETURN, KEY_ALT, KEY_TAB,
    MAX_KEY_INPUTS
} Keys;

typedef enum {
    MOUSE_LEFT, MOUSE_RIGHT,
    MAX_MOUSE_INPUTS
} Mouse;

typedef struct input Input;

Input *createInputTracker();

bool checkUserInput(Input *pInputs);

void inputHoldTimer(Input *pInputs);

bool checkKeyCombo(Input const *pInputs, int key1, int key2);

const int *getKeyInputs(Input const *pInputs);

const int *getMouseInputs(Input const *pInputs);

void destroyInputTracker(Input *pInputs);