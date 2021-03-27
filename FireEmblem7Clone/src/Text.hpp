#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <unordered_map>
#include <string>

enum Font
{
    Slim,
    SlimDark
};

enum Alignment
{
    Left,
    Center,
    Right
};

class Text
{
private:
    static SDL_Texture* texFontSlim;
    static SDL_Texture* texFontSlimDark;
    static std::unordered_map<char, SDL_Rect> asciiToRectSlim;
    static std::unordered_map<char, SDL_Rect> asciiToRectSlimDark;

    static void loadFont(Font fontToLoad);

public:
    static const SDL_Color White;
    static const SDL_Color Blue;
    static const SDL_Color Yellow;
    static SDL_Color Green;
    static void updateGreen();
    static void renderText(std::string text, Font font, SDL_Color color, int x, int y, Alignment alignment, int width);
};
