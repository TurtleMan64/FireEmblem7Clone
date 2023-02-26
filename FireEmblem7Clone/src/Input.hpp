#pragma once

#include <SDL/SDL.h>

class Input
{
private:
    static Uint8 inputs[9];
    static Uint8 inputsPrev[9];
    static int timeUntilNextAutoInput;
    static SDL_GameController* controller;

public:
    static void init();
    static void poll();
     
    static bool pressedA();
    static bool pressedB();
    static bool pressedX();
    static bool pressedY();
     
    static bool pressedStart();
     
    static bool pressedUp();
    static bool pressedDown();
    static bool pressedLeft();
    static bool pressedRight();

    static bool pressingA();
    static bool pressingB();
    static bool pressingX();
    static bool pressingY();
};
