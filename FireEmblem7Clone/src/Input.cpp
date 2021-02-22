#include <SDL/SDL.h>

#include <cstdio>

#include "Input.hpp"
#include "Global.hpp"

Uint8 Input::inputs[9]     = {0,0,0,0,0,0,0,0,0};
Uint8 Input::inputsPrev[9] = {0,0,0,0,0,0,0,0,0};
int Input::timeUntilNextAutoInput = 0;

void Input::poll()
{
    SDL_memcpy(Input::inputsPrev, Input::inputs, 9);
    const Uint8* keyboard = SDL_GetKeyboardState(nullptr);
    Input::inputs[0] = keyboard[SDL_SCANCODE_M];
    Input::inputs[1] = keyboard[SDL_SCANCODE_N];
    Input::inputs[2] = keyboard[SDL_SCANCODE_K];
    Input::inputs[3] = keyboard[SDL_SCANCODE_J];
    Input::inputs[4] = keyboard[SDL_SCANCODE_RETURN];
    Input::inputs[5] = keyboard[SDL_SCANCODE_W];
    Input::inputs[6] = keyboard[SDL_SCANCODE_S];
    Input::inputs[7] = keyboard[SDL_SCANCODE_A];
    Input::inputs[8] = keyboard[SDL_SCANCODE_D];

    if (Global::transitionTimer > 0 || Global::fadeInTimer > 0)
    {
        Input::inputs[0] = 0;
        Input::inputs[1] = 0;
        Input::inputs[2] = 0;
        Input::inputs[3] = 0;
        Input::inputs[4] = 0;
        Input::inputs[5] = 0;
        Input::inputs[6] = 0;
        Input::inputs[7] = 0;
        Input::inputs[8] = 0;
    }

    if (inputs[5] || inputs[6] || inputs[7] || inputs[8])
    {
        timeUntilNextAutoInput--;

        if (timeUntilNextAutoInput <= 0)
        {
            timeUntilNextAutoInput = 5;
        }
    }
    else
    {
        timeUntilNextAutoInput = 16;
    }
}

bool Input::pressedA()
{
    return inputs[0] && !inputsPrev[0];
}

bool Input::pressedB()
{
    return inputs[1] && !inputsPrev[1];
}

bool Input::pressedX()
{
    return inputs[2] && !inputsPrev[2];
}

bool Input::pressedY()
{
    return inputs[3] && !inputsPrev[3];
}

bool Input::pressedStart()
{
    return inputs[4] && !inputsPrev[4];
}

bool Input::pressedUp()
{
    if (!inputs[5])
    {
        return false;
    }
    if (timeUntilNextAutoInput == 1)
    {
        return true;
    }
    return !inputsPrev[5];
}

bool Input::pressedDown()
{
    if (!inputs[6])
    {
        return false;
    }
    if (timeUntilNextAutoInput == 1)
    {
        return true;
    }
    return !inputsPrev[6];
}

bool Input::pressedLeft()
{
    if (!inputs[7])
    {
        return false;
    }
    if (timeUntilNextAutoInput == 1)
    {
        return true;
    }
    return !inputsPrev[7];
}

bool Input::pressedRight()
{
    if (!inputs[8])
    {
        return false;
    }
    if (timeUntilNextAutoInput == 1)
    {
        return true;
    }
    return !inputsPrev[8];
}
