#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <unordered_map>
#include <string>

enum Font
{
    Border,
    Borderless
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
    static SDL_Texture* texFontBorder;
    static SDL_Texture* texFontBorderless;
    static std::unordered_map<char, SDL_Rect> asciiToRectBorder;
    static std::unordered_map<char, SDL_Rect> asciiToRectBorderless;

    static void loadFont(Font fontToLoad);

public:
    static const SDL_Color White;
    static const SDL_Color Blue;
    static const SDL_Color Yellow;
    static const SDL_Color DarkGrey;
    static SDL_Color Green;
    static void init();
    static void updateGreen();
    static void renderText(std::string text, Font font, SDL_Color color, int x, int y, Alignment alignment, int width);
};
