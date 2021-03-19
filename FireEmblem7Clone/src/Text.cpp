#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <unordered_map>
#include <vector>
#include <string>

#include "Global.hpp"
#include "Util.hpp"
#include "Text.hpp"

const SDL_Color Text::White {255, 255, 255, 255};
const SDL_Color Text::Blue  {198, 255, 255, 255};
const SDL_Color Text::Yellow{255, 247, 140, 255};

SDL_Texture* Text::texFontWhite = nullptr;
SDL_Texture* Text::texFontBlack = nullptr;
std::unordered_map<char, SDL_Rect> Text::asciiToRectWhite;
std::unordered_map<char, SDL_Rect> Text::asciiToRectBlack;

void Text::loadFont(Font fontToLoad)
{
    SDL_Texture** texToLoad = nullptr;
    std::unordered_map<char, SDL_Rect>* mapToLoad = nullptr;
    std::string name = "Dummy";

    switch (fontToLoad)
    {
        case Font::White:
            texToLoad = &texFontWhite;
            mapToLoad = &asciiToRectWhite;
            name = "White";
            break;

        case Font::Black:
            texToLoad = &texFontBlack;
            mapToLoad = &asciiToRectBlack;
            name = "Black";
            break;

        default:
            break;
    }

    std::string filenameFont = "res/Images/Text/Font" + name + ".png";
    (*texToLoad) = IMG_LoadTexture(Global::sdlRenderer, filenameFont.c_str());
    if ((*texToLoad) == nullptr)
    {
        printf("Error: Could not open file '%s'\n", filenameFont.c_str());
        return;
    }

    std::vector<std::string> file = Util::readFile("res/Text/Font" + name + ".fnt");

    for (int i = 0; i < 71; i++)
    {
        char ascii = file[i][0];
        int  width = file[i][2] - 48;

        SDL_Rect rect;
        rect.x = i*8;
        rect.y = 0;
        rect.w = width;
        rect.h = 16;

        mapToLoad->insert(std::pair<char, SDL_Rect>(ascii, rect));
    }
}

void Text::renderText(std::string text, Font font, SDL_Color color, int x, int y, Alignment alignment, int width)
{
    SDL_Texture** fontTex = nullptr;
    std::unordered_map<char, SDL_Rect>* asciiToRect = nullptr;

    switch (font)
    {
        case Font::White:
            fontTex = &texFontWhite;
            asciiToRect = &asciiToRectWhite;
            break;

        case Font::Black:
            fontTex = &texFontBlack;
            asciiToRect = &asciiToRectBlack;
            break;

        default:
            break;
    }

    if ((*fontTex) == nullptr)
    {
        loadFont(font);
    }

    int textWidth = 0;
    for (int i = 0; i < text.size(); i++)
    {
        char c = text[i];
        SDL_Rect letterRect = asciiToRect->at(c);
        textWidth += letterRect.w;
    }

    if (alignment == Left)
    {
        textWidth = 0;
        width = 0;
    }
    else if (alignment == Center)
    {
        textWidth = textWidth/2;
        width = width/2;
    }

    int xOff = (x + width) - textWidth;
    for (int i = 0; i < text.size(); i++)
    {
        char c = text[i];
        SDL_Rect letterRect = asciiToRect->at(c);

        SDL_Rect srcRect;
        srcRect.x = letterRect.x;
        srcRect.y = 0;
        srcRect.w = 8;
        srcRect.h = 16;

        SDL_Rect dstRect;
        dstRect.x = xOff;
        dstRect.y = y;
        dstRect.w = 8;
        dstRect.h = 16;

        xOff += letterRect.w;

        SDL_SetTextureColorMod((*fontTex), color.r, color.g, color.b);
        SDL_SetTextureAlphaMod((*fontTex), color.a);
        SDL_RenderCopy(Global::sdlRenderer, (*fontTex), &srcRect, &dstRect);
    }
}
