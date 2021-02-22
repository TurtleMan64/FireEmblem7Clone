#pragma once

#include <SDL/SDL.h>

class Input
{
private:
    static Uint8 inputs[9];
    static Uint8 inputsPrev[9];
    static int timeUntilNextAutoInput;

public:
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
};
