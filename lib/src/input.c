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

int keyInputs(Input *pInputs, SDL_Event event) {
    int pressedKey = -1;
    bool state = (event.type == SDL_KEYDOWN);
    switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_ESCAPE:
            pressedKey = KEY_ESCAPE;
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
        case SDL_SCANCODE_R:
            pressedKey = KEY_R;
            break;
        case SDL_SCANCODE_T:
            pressedKey = KEY_T;
            break;
        case SDL_SCANCODE_F:
            pressedKey = KEY_F;
            break;
        case SDL_SCANCODE_G:
            pressedKey = KEY_G;
            break;
        case SDL_SCANCODE_H:
            pressedKey = KEY_H;
            break;
        case SDL_SCANCODE_J:
            pressedKey = KEY_J;
            break;
        case SDL_SCANCODE_Q:
            pressedKey = KEY_Q;
            break;
        case SDL_SCANCODE_E:
            pressedKey = KEY_E;
            break;
        case SDL_SCANCODE_0:
            pressedKey = KEY_0;
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
        case SDL_SCANCODE_4:
            pressedKey = KEY_4;
            break;
        case SDL_SCANCODE_5:
            pressedKey = KEY_5;
            break;
        case SDL_SCANCODE_6:
            pressedKey = KEY_6;
            break;
        case SDL_SCANCODE_7:
            pressedKey = KEY_7;
            break;
        case SDL_SCANCODE_8:
            pressedKey = KEY_8;
            break;
        case SDL_SCANCODE_9:
            pressedKey = KEY_9;
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
        case SDL_SCANCODE_BACKSPACE:
            pressedKey = KEY_BACKSPACE;
            break;
    }

    return pressedKey;
}

int mouseInputs(Input *pInputs, SDL_Event event) {
    int pressedKey = -1;
    bool state = (event.type == SDL_MOUSEBUTTONDOWN);
    switch (event.button.button) {
        case SDL_BUTTON_LEFT:
            pressedKey = MOUSE_LEFT;
            break;
        case SDL_BUTTON_RIGHT:
            pressedKey = MOUSE_RIGHT;
            break;
    }

    return pressedKey;
}

bool checkUserInput(Input *pInputs) {
    SDL_Event event;
    bool gameRunning = true;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            gameRunning = false;
        }
        else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            int pressedKey = keyInputs(pInputs, event);

            if (pressedKey == -1) { continue; }

            if (pressedKey == KEY_ESCAPE) { gameRunning = false; continue; }

            switch (event.type) {
                case SDL_KEYDOWN:
                    if (pInputs->keys[pressedKey] == KEY_STATE_UP) { pInputs->keys[pressedKey] = KEY_STATE_DOWN; }
                    break;
                case SDL_KEYUP:
                    pInputs->keys[pressedKey] = KEY_STATE_UP;
                    break;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
            int pressedKey = mouseInputs(pInputs, event);

            if (pressedKey == -1) { continue; }

            switch (event.type) {
                case SDL_MOUSEBUTTONDOWN:
                    if (pInputs->mouse[pressedKey] == KEY_STATE_UP) { pInputs->mouse[pressedKey] = KEY_STATE_DOWN; }
                    break;
                case SDL_MOUSEBUTTONUP:
                    pInputs->mouse[pressedKey] = KEY_STATE_UP;
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

int getKeyState(Input const *pInputs, int keyboardButton) {
    if (keyboardButton < 0 || keyboardButton >= MAX_KEY_INPUTS) { return false; }

    return pInputs->keys[keyboardButton];
}

int getMouseState(Input const *pInputs, int mouseButton) {
    if (mouseButton < 0 || mouseButton >= MAX_MOUSE_INPUTS) { return false; }

    return pInputs->mouse[mouseButton];
}

bool checkKeyCombo(Input const *pInputs, int key1, int key2) {
    if (key1 < 0 || key1 >= MAX_KEY_INPUTS) { return false; }
    if (key2 < 0 || key2 >= MAX_KEY_INPUTS) { return false; }

    if (pInputs->keys[key1] == KEY_STATE_DOWN && (pInputs->keys[key2] >= KEY_STATE_DOWN && pInputs->keys[key2] <= KEY_STATE_HOLD)) { return true; }

    if (pInputs->keys[key2] == KEY_STATE_DOWN && (pInputs->keys[key1] >= KEY_STATE_DOWN && pInputs->keys[key1] <= KEY_STATE_HOLD)) { return true; }

    return false;
}

void destroyInputTracker(Input *pInputs) {
    if (pInputs == NULL) { return; }

    free(pInputs);
    return;
}