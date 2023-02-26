#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

#include <string>
#include <chrono>

#include "Global.hpp"
#include "Input.hpp"
#include "ImageAnimation.hpp"
#include "Map.hpp"
#include "MapTile.hpp"
#include "MainMenu.hpp"
#include "ClassData.hpp"
#include "UnitDisplay.hpp"
#include "Text.hpp"
#include "Djikstra.hpp"
#include "Items.hpp"
#include "WindowBox.hpp"
#include "Audio.hpp"
#include "Usleep.hpp"

SDL_Renderer* Global::sdlRenderer = nullptr;

int Global::frameCount = 0;

Global::GameState Global::gameState = Title;

int Global::mapId = 0;

int Global::transitionTimerMax = 30;
int Global::transitionTimer = 0;
Global::GameState Global::transitionState = Title;
int Global::fadeInTimer = 30;

std::chrono::time_point<std::chrono::steady_clock> Global::previousFrameTimestamp;

//#undef main
int main(int argc, char* argv[])
{
    argc;
    argv;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_AUDIO);
    SDL_Init(SDL_INIT_GAMECONTROLLER);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    bool vsync = true;

    int vsyncFlag = SDL_RENDERER_PRESENTVSYNC;
    if (!vsync)
    {
        vsyncFlag = 0;
    }

    SDL_Window* window = SDL_CreateWindow("Fire Emblem: The Blazing Blade", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 240, 160, SDL_WINDOW_RESIZABLE);

    Global::sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | vsyncFlag);

    SDL_Texture* mainRenderTexture = SDL_CreateTexture(Global::sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 240, 160);

    //Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    //Mix_Chunk* jumpEffect = Mix_LoadWAV("res/Audio/SFX/LevelUp.wav");
    //Mix_PlayChannel(-1, jumpEffect, 0);

    TTF_Font* fontConsolas = TTF_OpenFont("res/Text/CONSOLA.TTF", 48);

    //https://stackoverflow.com/questions/3229391/sdl-sdl-ttf-transparent-blended-text
    SDL_Color textColorFg = {255, 255, 255, 0};
    SDL_Color textColorBg = {0, 0, 0, 0};
    SDL_Rect textRect{0,0,0,0};
    SDL_Surface* surface = TTF_RenderText_LCD(fontConsolas, "test font render", textColorFg, textColorBg);
    SDL_Texture* renderedFontTexture = SDL_CreateTextureFromSurface(Global::sdlRenderer, surface);
    textRect.w = surface->w;
    textRect.h = surface->h;
    SDL_FreeSurface(surface);

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
    MapTile::init();
    Usleep_init();

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

    int animIndex = 0;
    bool running = true;
    while (running)
    {
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

        Global::limitTo60FPS();

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

        int windowFlags = SDL_GetWindowFlags(window);
        // This prevents a strange memory leak that happens somewhere in SDL2.dll.
        // When the game is minimized, memory climbs steadily. Can prevent by not calling
        // any SDL rendering functions when minimized. This also "pauses" the game since
        // the game logic is skipped.
        if (windowFlags & SDL_WINDOW_MINIMIZED)
        {
            continue;
        }

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

        SDL_SetRenderTarget(Global::sdlRenderer, nullptr); //render the GBA sized texture to the main window

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

        //SDL_RenderCopy(Global::sdlRenderer, renderedFontTexture, NULL, &textRect);
        //WindowBox::render(0, 0, 10, 15);
        //Text::renderText("Path of Radiance Ironman", Font::Border, SDL_Color{255, 255, 255, 255}, 0, 0, Alignment::Left, 0);

        SDL_RenderPresent(Global::sdlRenderer);

        animIndex++;
    }

    TTF_CloseFont(fontConsolas);

    SDL_DestroyRenderer(Global::sdlRenderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
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

void Global::limitTo60FPS()
{
    std::chrono::time_point<std::chrono::steady_clock> currentFrameTimestamp = std::chrono::high_resolution_clock::now();

    long long frameTimeMicro = std::chrono::duration_cast<std::chrono::microseconds>(currentFrameTimestamp - Global::previousFrameTimestamp).count();

    if (frameTimeMicro < 16666)
    {
        int dtFrameNeedsToTakeMicro = 16666;

        bool slept = false;

        // First, see if we can sleep some of the time to avoid cpu spinning.
        const int sleepBufferMicro = 2000; //sleep will hopefully never take longer than this to return (2 milliseconds)
        int sleepTimeMicro = (int)((int)dtFrameNeedsToTakeMicro - (int)frameTimeMicro) - sleepBufferMicro;
        if (sleepTimeMicro >= 1)
        {
            usleep(sleepTimeMicro);
            slept = true;
        }

        bool spinned = false;
        
        // Next, spin the CPU to precisely wait the correct amount of time.
        currentFrameTimestamp = std::chrono::high_resolution_clock::now();
        while (std::chrono::duration_cast<std::chrono::microseconds>(currentFrameTimestamp - Global::previousFrameTimestamp).count() < dtFrameNeedsToTakeMicro)
        {
            currentFrameTimestamp = std::chrono::high_resolution_clock::now();
            spinned = true;
        }

        if (slept && !spinned)
        {
            printf("slept for too long! (%lld microseconds too much)\n", (std::chrono::duration_cast<std::chrono::microseconds>(currentFrameTimestamp - Global::previousFrameTimestamp).count()) - dtFrameNeedsToTakeMicro);
        }
        else if (!slept && !spinned)
        {
            //printf("frame has already taken too long!\n");
        }
    }

    Global::previousFrameTimestamp = currentFrameTimestamp;
}
