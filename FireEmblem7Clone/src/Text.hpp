#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <unordered_map>
#include <string>

enum Font
{
    White,
    Black
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
    static SDL_Texture* texFontWhite;
    static SDL_Texture* texFontBlack;
    static std::unordered_map<char, SDL_Rect> asciiToRectWhite;
    static std::unordered_map<char, SDL_Rect> asciiToRectBlack;

    static void loadFont(Font fontToLoad);

public:
    static const SDL_Color White;
    static const SDL_Color Blue;
    static const SDL_Color Yellow;
    static void renderText(std::string text, Font font, SDL_Color color, int x, int y, Alignment alignment, int width);
};
