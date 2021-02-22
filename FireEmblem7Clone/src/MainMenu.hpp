#pragma once

#include <SDL/SDL.h>

#include "Global.hpp"
#include "Input.hpp"

class Sprite;

class MainMenu
{
private:
    static SDL_Texture* texBackground;
    static Sprite* buttonResumeChapter;
    static Sprite* buttonRestartChapter;
    static Sprite* buttonNewGame;
    static Sprite* buttonExtras;
    static Sprite* buttonSelectL;
    static Sprite* buttonSelectR;

    static int buttonIdx;

public:
    static void init();

    static void step();
};
