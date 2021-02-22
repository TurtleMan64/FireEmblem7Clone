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

class Text
{
private:
    static SDL_Texture* texFontWhite;
    static SDL_Texture* texFontBlack;
    static std::unordered_map<char, SDL_Rect> asciiToRectWhite;
    static std::unordered_map<char, SDL_Rect> asciiToRectBlack;

    static void loadFont(Font fontToLoad);

public:
    static void renderText(std::string text, Font font, SDL_Color color, int x, int y, bool centered, int width, int padding);
};
