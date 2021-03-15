#include <SDL/SDL.h>

#include <cstdio>

#include "Input.hpp"
#include "Global.hpp"

Uint8 Input::inputs[9]     = {0,0,0,0,0,0,0,0,0};
Uint8 Input::inputsPrev[9] = {0,0,0,0,0,0,0,0,0};
int Input::timeUntilNextAutoInput = 0;
SDL_GameController* Input::controller = nullptr;

void Input::init()
{
    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        if (SDL_IsGameController(i))
        {
            controller = SDL_GameControllerOpen(i);
            if (controller != nullptr)
            {
                break;
            }
            else
            {
                printf("Could not open gamecontroller %i: %s\n", i, SDL_GetError());
            }
        }
    }
}

void Input::poll()
{
    SDL_memcpy(inputsPrev, inputs, 9);

    //Update from keyboard
    const Uint8* keyboard = SDL_GetKeyboardState(nullptr);
    inputs[0] = keyboard[SDL_SCANCODE_M];
    inputs[1] = keyboard[SDL_SCANCODE_N];
    inputs[2] = keyboard[SDL_SCANCODE_K];
    inputs[3] = keyboard[SDL_SCANCODE_J];
    inputs[4] = keyboard[SDL_SCANCODE_RETURN];
    inputs[5] = keyboard[SDL_SCANCODE_W];
    inputs[6] = keyboard[SDL_SCANCODE_S];
    inputs[7] = keyboard[SDL_SCANCODE_A];
    inputs[8] = keyboard[SDL_SCANCODE_D];

    //Update from gamepad
    if (controller != nullptr)
    {
        SDL_GameControllerUpdate();

        inputs[0] = inputs[0] | SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A);
        inputs[1] = inputs[1] | SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X);
        inputs[2] = inputs[2] | SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B);
        inputs[3] = inputs[3] | SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y);
        inputs[4] = inputs[4] | SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START);
        inputs[5] = inputs[5] | SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP);
        inputs[6] = inputs[6] | SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        inputs[7] = inputs[7] | SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        inputs[8] = inputs[8] | SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    }

    if (Global::transitionTimer > 0 || Global::fadeInTimer > 0)
    {
        inputs[0] = 0;
        inputs[1] = 0;
        inputs[2] = 0;
        inputs[3] = 0;
        inputs[4] = 0;
        inputs[5] = 0;
        inputs[6] = 0;
        inputs[7] = 0;
        inputs[8] = 0;
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
        timeUntilNextAutoInput = 20;
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
