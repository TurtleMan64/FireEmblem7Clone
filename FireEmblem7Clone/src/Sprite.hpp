#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <string>
#include <unordered_map>

class ImageAnimation;

class Sprite
{
private:
    static std::unordered_map<std::string, int> pathToCount;
    static std::unordered_map<std::string, ImageAnimation*> pathToImage;

    std::string imagePath;
    ImageAnimation* image = nullptr;

public:
    int x = 0;
    int y = 0;
    int scaleX = 1; //should only be 1 or -1
    int scaleY = 1; //should only be 1 or -1
    int imageIndex = 0;

    Sprite(std::string path, int spawnX, int spawnY, bool swapPalette);
    ~Sprite();

    void render();

    void render(SDL_Color color);
};
