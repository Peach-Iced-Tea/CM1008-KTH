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
    KEY_SPACE, KEY_COMMA, KEY_PERIOD, KEY_RETURN, KEY_ALT, KEY_TAB, KEY_ESCAPE,
    MAX_KEY_INPUTS
} Keys;

typedef enum {
    MOUSE_LEFT, MOUSE_RIGHT,
    MAX_MOUSE_INPUTS
} Mouse;

typedef struct input Input;

Input *createInputTracker();

/*
Use this function to check which user input events that SDL_PollEvent has found.
*/
bool checkUserInput(Input *inputs);

/*
This function counts how many cycles the key/mouse button has been held down.

If it equals KEY_STATE_HOLD then the button is considered to be held down.
*/
void inputHoldTimer(Input *inputs);

/*
Check the state of a specific button tied to the keyboard input list.
*/
int getKeyState(Input const *inputs, int keyboardButton);

/*
Check the state of a specific button tied to the mouse input list.
*/
int getMouseState(Input const *inputs, int mouseButton);

/*
Check a combination of two keys related to the keyboard input list to see if the have both been pressed down.
*/
bool checkKeyCombo(Input const *inputs, int key1, int key2);

/*
Use this function to destroy the given Input pointer and free up memory.
*/
void destroyInputTracker(Input *inputs);