#include "input.h"

struct input {
    int keys[MAX_KEY_INPUTS];
    int mouse[MAX_MOUSE_INPUTS];
};

Input *createInputTracker() {
    Input *pInput = malloc(sizeof(Input));
    for (int i = 0; i < MAX_KEY_INPUTS; i++) {
        pInput->keys[i] = 0;
    }

    for (int i = 0; i < MAX_MOUSE_INPUTS; i++) {
        pInput->mouse[i] = 0;
    }

    return pInput;
}

bool checkUserInput(Input *pInputs) {
    SDL_Event event;
    bool gameRunning = true;
    while(SDL_PollEvent(&event)) {
        int pressedKey = -1;
        if (event.type == SDL_QUIT) {
            gameRunning = false;
        }
        else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            bool state = (event.type == SDL_KEYDOWN);
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    gameRunning = false;
                    break;
                case SDL_SCANCODE_W:
                    pressedKey = KEY_W;
                    break;
                case SDL_SCANCODE_A:
                    pressedKey = KEY_A;
                    break;
                case SDL_SCANCODE_S:
                    pressedKey = KEY_S;
                    break;
                case SDL_SCANCODE_D:
                    pressedKey = KEY_D;
                    break;
                case SDL_SCANCODE_G:
                    pressedKey = KEY_G;
                    break;
                case SDL_SCANCODE_Q:
                    pressedKey = KEY_Q;
                    break;
                case SDL_SCANCODE_E:
                    pressedKey = KEY_E;
                    break;
                case SDL_SCANCODE_1:
                    pressedKey = KEY_1;
                    break;
                case SDL_SCANCODE_2:
                    pressedKey = KEY_2;
                    break;
                case SDL_SCANCODE_3:
                    pressedKey = KEY_3;
                    break;
                case SDL_SCANCODE_SPACE:
                    pressedKey = KEY_SPACE;
                    break;
                case SDL_SCANCODE_COMMA:
                    pressedKey = KEY_COMMA;
                    break;
                case SDL_SCANCODE_PERIOD:
                    pressedKey = KEY_PERIOD;
                    break;
                case SDL_SCANCODE_RETURN:
                    pressedKey = KEY_RETURN;
                    break;
                case SDL_SCANCODE_LALT:
                case SDL_SCANCODE_RALT:
                    pressedKey = KEY_ALT;
                    break;
                case SDL_SCANCODE_TAB:
                    pressedKey = KEY_TAB;
                    break;
            }

            if (pressedKey == -1) { continue; }

            switch (state) {
                case KEY_STATE_DOWN:
                    if (pInputs->keys[pressedKey] == KEY_STATE_UP) { pInputs->keys[pressedKey] = state; }
                    break;
                case KEY_STATE_UP:
                    pInputs->keys[pressedKey] = state;
                    break;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
            bool state = (event.type == SDL_MOUSEBUTTONDOWN);
            switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    pressedKey = MOUSE_LEFT;
                    break;
                case SDL_BUTTON_RIGHT:
                    pressedKey = MOUSE_RIGHT;
                    break;
            }

            if (pressedKey == -1) { continue; }

            switch (state) {
                case KEY_STATE_DOWN:
                    if (pInputs->mouse[pressedKey] == KEY_STATE_UP) { pInputs->mouse[pressedKey] = state; }
                    break;
                case KEY_STATE_UP:
                    pInputs->mouse[pressedKey] = state;
                    break;
            }
        }
    }

    return gameRunning;
}

void inputHoldTimer(Input *pInputs) {
    for (int i = 0; i < MAX_KEY_INPUTS; i++) {
        if (!pInputs->keys[i]) { continue; }

        if (pInputs->keys[i] < KEY_STATE_HOLD) { pInputs->keys[i]++; }
    }

    for (int i = 0; i < MAX_MOUSE_INPUTS; i++) {
        if (!pInputs->mouse[i]) { continue; }

        if (pInputs->mouse[i] < KEY_STATE_HOLD) { pInputs->mouse[i]++; }
    }

    return;
}

bool checkKeyCombo(Input const *pInputs, int key1, int key2) {
    if (key1 < 0 || key1 >= MAX_KEY_INPUTS) { return false; }
    if (key2 < 0 || key2 >= MAX_KEY_INPUTS) { return false; }

    if (pInputs->keys[key1] == KEY_STATE_DOWN && (pInputs->keys[key2] >= KEY_STATE_DOWN && pInputs->keys[key2] < KEY_STATE_HOLD)) { return true; }

    if (pInputs->keys[key2] == KEY_STATE_DOWN && (pInputs->keys[key1] >= KEY_STATE_DOWN && pInputs->keys[key1] < KEY_STATE_HOLD)) { return true; }

    return false;
}

const int *getKeyInputs(Input const *pInputs) {
    return pInputs->keys;
}

const int *getMouseInputs(Input const *pInputs) {
    return pInputs->mouse;
}

void destroyInputTracker(Input *pInputs) {
    if (pInputs == NULL) { return; }

    free(pInputs);
    return;
}