#pragma once

#include <SDL/SDL.h>
#include <chrono>

class Global
{
public:
    static SDL_Renderer* sdlRenderer;

    enum GameState {
        Title,
        MainMenu,
        Cutscene,
        Map,
        UnitDisplay,
        Battle
    };

    static int frameCount;

    static GameState gameState;

    static int mapId;
    
    static int transitionTimerMax;
    static int transitionTimer;
    static GameState transitionState;
    static int fadeInTimer;

    static void transitionToNewState(GameState newState, int transitionTime);

    static void limitTo60FPS();

private:
    static std::chrono::time_point<std::chrono::steady_clock> previousFrameTimestamp;
};
