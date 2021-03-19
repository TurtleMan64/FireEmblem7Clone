#pragma once

#include <SDL/SDL.h>

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
};
