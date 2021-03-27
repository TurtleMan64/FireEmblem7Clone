#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <unordered_map>
#include <vector>
#include <string>
#include <math.h>

#include "Global.hpp"
#include "Util.hpp"
#include "Text.hpp"

const SDL_Color Text::White {255, 255, 255, 255};
const SDL_Color Text::Blue  {198, 255, 255, 255};
const SDL_Color Text::Yellow{255, 247, 140, 255};
      SDL_Color Text::Green { 74, 239,  33, 255};

SDL_Texture* Text::texFontSlim     = nullptr;
SDL_Texture* Text::texFontSlimDark = nullptr;
std::unordered_map<char, SDL_Rect> Text::asciiToRectSlim;
std::unordered_map<char, SDL_Rect> Text::asciiToRectSlimDark;

void Text::loadFont(Font fontToLoad)
{
    SDL_Texture** texToLoad = nullptr;
    std::unordered_map<char, SDL_Rect>* mapToLoad = nullptr;
    std::string name = "Dummy";

    switch (fontToLoad)
    {
        case Font::Slim:
            texToLoad = &texFontSlim;
            mapToLoad = &asciiToRectSlim;
            name = "Slim";
            break;

        case Font::SlimDark:
            texToLoad = &texFontSlimDark;
            mapToLoad = &asciiToRectSlimDark;
            name = "SlimDark";
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
        case Font::Slim:
            fontTex = &texFontSlim;
            asciiToRect = &asciiToRectSlim;
            break;

        case Font::SlimDark:
            fontTex = &texFontSlimDark;
            asciiToRect = &asciiToRectSlimDark;
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

void Text::updateGreen()
{
    SDL_Color D = { 74, 239,  33, 255}; //dark  74, 239,  33, 255
    SDL_Color L = {198, 255, 173, 255}; //light 247, 255, 231, 255
    float t = Global::frameCount/10.0f;
    float amp = 0.5f + 0.5f*sinf(t);

    SDL_Color diff = {(Uint8)(L.r - D.r), (Uint8)(L.g - D.g), (Uint8)(L.b - D.b), (Uint8)255};
    SDL_Color toAdd = {(Uint8)(diff.r*amp), (Uint8)(diff.g*amp), (Uint8)(diff.b*amp), (Uint8)255};

    Green = {(Uint8)(D.r + toAdd.r), (Uint8)(D.g + toAdd.g), (Uint8)(D.b + toAdd.b), (Uint8)255};
}
