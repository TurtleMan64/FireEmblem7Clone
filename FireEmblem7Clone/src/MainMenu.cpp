#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "Global.hpp"
#include "Input.hpp"
#include "Sprite.hpp"
#include "Map.hpp"
#include "MainMenu.hpp"

SDL_Texture* MainMenu::texBackground   = nullptr;
Sprite* MainMenu::buttonResumeChapter  = nullptr;
Sprite* MainMenu::buttonRestartChapter = nullptr;
Sprite* MainMenu::buttonNewGame        = nullptr;
Sprite* MainMenu::buttonExtras         = nullptr;
Sprite* MainMenu::buttonSelectL        = nullptr;
Sprite* MainMenu::buttonSelectR        = nullptr;

int MainMenu::buttonIdx = 0;

void MainMenu::init()
{
    texBackground = IMG_LoadTexture(Global::sdlRenderer, "res/Images/BG/MainMenu.png");

    buttonResumeChapter  = new Sprite("res/Images/Sprites/MainMenu/ResumeChapter", 56, 24, false);
    buttonRestartChapter = new Sprite("res/Images/Sprites/MainMenu/RestartChapter", 56, 48, false);
    buttonNewGame        = new Sprite("res/Images/Sprites/MainMenu/NewGame", 56, 72, false);
    buttonExtras         = new Sprite("res/Images/Sprites/MainMenu/Extras", 56, 96, false);
    buttonSelectL        = new Sprite("res/Images/Sprites/MainMenu/Dragon", 56-16, 120, false);
    buttonSelectR        = new Sprite("res/Images/Sprites/MainMenu/Dragon", 240-56-16, 120, false);
    buttonSelectR->scaleX = -1;
}

void MainMenu::step()
{
    SDL_RenderCopy(Global::sdlRenderer, texBackground, nullptr, nullptr);

    if (Input::pressedUp())
    {
        buttonIdx--;
        if (buttonIdx < 0)
        {
            buttonIdx = 3;
        }
    }
    else if (Input::pressedDown())
    {
        buttonIdx = (buttonIdx + 1) % 4;
    }
    else if (Input::pressedA())
    {
        Global::transitionToNewState(Global::GameState::Map, 45);
        Map::loadFresh(13);//"res/Images/Maps/Map12.png");
    }
    else if (Input::pressedB())
    {
        Global::transitionToNewState(Global::GameState::Title, 30);
    }

    buttonSelectL->y = 20 + buttonIdx*24;
    buttonSelectR->y = 20 + buttonIdx*24;

    buttonResumeChapter->render();
    buttonRestartChapter->render();
    buttonNewGame->render();
    buttonExtras->render();
    buttonSelectL->render();
    buttonSelectR->render();
}
