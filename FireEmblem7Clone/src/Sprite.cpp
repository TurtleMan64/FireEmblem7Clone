#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <string>
#include <unordered_map>

#include "Global.hpp"
#include "ImageAnimation.hpp"
#include "Sprite.hpp"

std::unordered_map<std::string, int> Sprite::pathToCount;
std::unordered_map<std::string, ImageAnimation*> Sprite::pathToImage;

Sprite::Sprite(std::string path, int spawnX, int spawnY, bool swapPalette)
{
    x = spawnX;
    y = spawnY;

    if (!swapPalette)
    {
        imagePath = path;
    }
    else
    {
        imagePath = path + "-r";
    }

    auto result = Sprite::pathToCount.find(imagePath);
    if (result == Sprite::pathToCount.end() || result->second == 0) //new sprite
    {
        image = new ImageAnimation(path, swapPalette);

        Sprite::pathToImage[imagePath] = image;
        Sprite::pathToCount[imagePath] = 1;
    }
    else //reuse existing sprite
    {
        image = Sprite::pathToImage[imagePath];
        Sprite::pathToCount[imagePath] = Sprite::pathToCount[imagePath] + 1;
    }
}

Sprite::~Sprite()
{
    int count = Sprite::pathToCount[imagePath];
    Sprite::pathToCount[imagePath] = count - 1;
    if (count == 1)
    {
        ImageAnimation* imageToDelete = Sprite::pathToImage[imagePath];
        Sprite::pathToImage.erase(imagePath);
        delete imageToDelete;
    }
}

void Sprite::render()
{
    SDL_Rect rect;
    rect.x = x - image->originX;
    rect.y = y - image->originY;
    rect.w = image->width;
    rect.h = image->height;

    SDL_RendererFlip flip = SDL_FLIP_NONE;

    if (scaleX < 0)
    {
        flip = SDL_FLIP_HORIZONTAL;
    }

    if (scaleY < 0)
    {
        flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);
    }

    SDL_Point center;
    center.x = x + image->width/2;
    center.y = y + image->height/2;

    SDL_RenderCopyEx(Global::sdlRenderer, image->getTexture(imageIndex), nullptr, &rect, 0.0, &center, flip);
}

void Sprite::render(SDL_Color color)
{
    SDL_Rect rect;
    rect.x = x - image->originX;
    rect.y = y - image->originY;
    rect.w = image->width;
    rect.h = image->height;

    SDL_RendererFlip flip = SDL_FLIP_NONE;

    if (scaleX < 0)
    {
        flip = SDL_FLIP_HORIZONTAL;
    }

    if (scaleY < 0)
    {
        flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);
    }

    SDL_Point center;
    center.x = x + image->width/2;
    center.y = y + image->height/2;

    SDL_Texture* tex = image->getTexture(imageIndex);

    SDL_SetTextureColorMod(tex, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(tex, color.a);

    SDL_RenderCopyEx(Global::sdlRenderer, tex, nullptr, &rect, 0.0, &center, flip);
}
