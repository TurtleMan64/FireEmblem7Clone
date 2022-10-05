#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

#include <string>

#include "Global.hpp"
#include "Input.hpp"
#include "ImageAnimation.hpp"
#include "Map.hpp"
#include "MainMenu.hpp"
#include "ClassData.hpp"
#include "UnitDisplay.hpp"
#include "Text.hpp"
#include "Djikstra.hpp"
#include "Items.hpp"
#include "WindowBox.hpp"
#include "Audio.hpp"

SDL_Renderer* Global::sdlRenderer = nullptr;

float Global::fpsSleepBias = 16.0f;
int Global::frameCount = 0;

Global::GameState Global::gameState = Title;

int Global::mapId = 0;

int Global::transitionTimerMax = 30;
int Global::transitionTimer = 0;
Global::GameState Global::transitionState = Title;
int Global::fadeInTimer = 30;

#undef main
int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_AUDIO);
    SDL_Init(SDL_INIT_GAMECONTROLLER);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("FE7", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 240, 160, SDL_WINDOW_RESIZABLE);

    Global::sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);

    SDL_Texture* mainRenderTexture = SDL_CreateTexture(Global::sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 240, 160);

    //Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    //Mix_Chunk* jumpEffect = Mix_LoadWAV("res/Audio/SFX/LevelUp.wav");
    //Mix_PlayChannel(-1, jumpEffect, 0);

    Audio::init();
    MainMenu::init();
    Input::init();
    ClassData::init();
    Djikstra::init();
    Item::init();
    Map::init();
    Text::init();
    UnitDisplay::init();
    WindowBox::init();

    //SDL_SetHintWithPriority(SDL_HINT_RENDER_VSYNC, "0", SDL_HINT_OVERRIDE);
    //SDL_GL_SetSwapInterval(0);
    SDL_SetRenderDrawColor(Global::sdlRenderer, 0, 0, 0, 0);
    SDL_SetRenderDrawBlendMode(Global::sdlRenderer, SDL_BLENDMODE_BLEND);
    //SDL_Surface* surface = SDL_GetWindowSurface(window);

    SDL_Rect rectEntireScreen;
    rectEntireScreen.x = 0;
    rectEntireScreen.y = 0;
    rectEntireScreen.w = 240;
    rectEntireScreen.h = 160;

    //ImageAnimation animation("res/Images/Sprites/Battle/SwordMaster");

    SDL_Texture* texTitleScreen = IMG_LoadTexture(Global::sdlRenderer, "res/Images/BG/TitleScreen.png");
    //SDL_Texture* texMainMenu    = IMG_LoadTexture(Global::sdlRenderer, "res/Images/BG/MainMenu.png");

    float prevTimeMs = (float)SDL_GetTicks();
    float prevSecond = prevTimeMs/1000.0f;
    int framesThisSecond = 0;

    int animIndex = 0;
    bool running = true;
    while (running)
    {
        framesThisSecond++;
        Global::frameCount++;

        if (Global::fadeInTimer > 0)
        {
            Global::fadeInTimer--;
        }

        if (Global::transitionTimer > 0)
        {
            Global::transitionTimer--;

            if (Global::transitionTimer == 0)
            {
                Global::fadeInTimer = Global::transitionTimerMax;
                Global::gameState = Global::transitionState;
            }
        }

        float currentTimeMs = (float)SDL_GetTicks();
        //if its been < 10ms since last frame, that means vsync isnt on and the game is probably running really fast.
        // this would also happen with a 120hz or 144hz etc monitor. it also seems to happen when you minimize the 
        // game window. in either case we dont want the game wasting useless cpu + gpu so we can sleep for approx the
        // remaining time. fps wont be a consistent 60 but the game is probably minimized so we dont really care.
        if (currentTimeMs - prevTimeMs < 10)
        {
            //printf("Frame only took %dms, spin locking for rest of the time\n", (currentTime - prevTime));
            //while (currentTime - prevTime < 16)
            {
                //LOL
                //currentTime = SDL_GetTicks();
            }
            SDL_Delay((16 - (int)(currentTimeMs - prevTimeMs)) + (int)Global::fpsSleepBias);
            //printf("Sleeping for %dms\n", (16 - (int)(currentTimeMs - prevTimeMs)) + (int)Global::fpsSleepBias);
        }
        prevTimeMs = currentTimeMs;

        if (currentTimeMs/1000 > prevSecond)
        {
            //printf("fps = %d, bias = %f\n", framesThisSecond, Global::fpsSleepBias);

            Global::fpsSleepBias += 0.24f*(framesThisSecond - 60);

            if (Global::fpsSleepBias < 0.0f)
            {
                Global::fpsSleepBias = 0.0f;
            }

            framesThisSecond = 0;
            prevSecond += 1.0f;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
        
                default:
                    break;
            }
        }

        Input::poll();

        SDL_SetRenderTarget(Global::sdlRenderer, mainRenderTexture); //render to the GBA sized texture
        SDL_RenderClear(Global::sdlRenderer);

        Text::updateGreen();

        switch (Global::gameState)
        {
            case Global::Title:
                SDL_RenderCopy(Global::sdlRenderer, texTitleScreen, nullptr, nullptr);
                if (Input::pressedStart() || Input::pressedA())
                {
                    Global::transitionToNewState(Global::GameState::MainMenu, 30);
                }
                break;

            case Global::MainMenu:
                MainMenu::step();
                break;

            case Global::Cutscene:

                break;

            case Global::Map:
                Map::step();
                break;

            case Global::UnitDisplay:
                UnitDisplay::step();
                break;

            case Global::Battle:

                break;

            default:
                break;
        }

        //Render a fadeout/fadein
        if (Global::fadeInTimer > 0)
        {
            SDL_SetRenderDrawColor(Global::sdlRenderer, 0, 0, 0, (Uint8)(Global::fadeInTimer*(255.0f/Global::transitionTimerMax)));
            SDL_RenderFillRect(Global::sdlRenderer, &rectEntireScreen);
        }

        if (Global::transitionTimer > 0)
        {
            SDL_SetRenderDrawColor(Global::sdlRenderer, 0, 0, 0, 255 - (Uint8)(Global::transitionTimer*(255.0f/Global::transitionTimerMax)));
            SDL_RenderFillRect(Global::sdlRenderer, &rectEntireScreen);
        }

        SDL_SetRenderTarget(Global::sdlRenderer, nullptr); //render to main window

        bool integerScaling = true;

        int windowW;
        int windowH;
        SDL_GetWindowSize(window, &windowW, &windowH);

        int scaleH = windowH/160;
        int scaleW = windowW/240;

        int scale = scaleH;
        if (scaleW < scaleH)
        {
            scale = scaleW;
        }

        if (integerScaling && scale > 0)
        {
            SDL_Rect gbaImageRect;
            gbaImageRect.x = (windowW - 240*scale)/2;
            gbaImageRect.y = (windowH - 160*scale)/2;
            gbaImageRect.w = 240*scale;
            gbaImageRect.h = 160*scale;
            SDL_RenderCopy(Global::sdlRenderer, mainRenderTexture, nullptr, &gbaImageRect);
        }
        else
        {
            float ratio = ((float)windowW)/windowH;
            if (ratio == 1.5f)
            {
                SDL_RenderCopy(Global::sdlRenderer, mainRenderTexture, nullptr, nullptr);
            }
            else if (ratio > 1.5f)
            {
                int desiredWidth = (int)(windowH*1.5f);
                SDL_Rect gbaImageRect;
                gbaImageRect.x = (windowW - desiredWidth)/2;
                gbaImageRect.y = 0;
                gbaImageRect.w = desiredWidth;
                gbaImageRect.h = windowH;
                SDL_RenderCopy(Global::sdlRenderer, mainRenderTexture, nullptr, &gbaImageRect);
            }
            else
            {
                int desiredHeight = (int)(windowW/1.5f);
                SDL_Rect gbaImageRect;
                gbaImageRect.x = 0;
                gbaImageRect.y = (windowH - desiredHeight)/2;
                gbaImageRect.w = windowW;
                gbaImageRect.h = desiredHeight;
                SDL_RenderCopy(Global::sdlRenderer, mainRenderTexture, nullptr, &gbaImageRect);
            }
        }

        SDL_RenderPresent(Global::sdlRenderer);

        animIndex++;
    }

    SDL_DestroyRenderer(Global::sdlRenderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    //Mix_FreeChunk(jumpEffect);
    SDL_Quit();

    return 0;
}

void Global::transitionToNewState(GameState newState, int transitionTime)
{
    Global::transitionState = newState;
    Global::transitionTimer = transitionTime;
    Global::transitionTimerMax = transitionTime;
}
