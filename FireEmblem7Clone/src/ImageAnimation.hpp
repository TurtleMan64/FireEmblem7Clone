#pragma once

#include <SDL/SDL.h>

#include <string>
#include <vector>
#include <unordered_map>

class ImageAnimation
{
private:
    std::vector<SDL_Texture*> images;
    std::unordered_map<int, int> frameToTexture;
    int totalAnimationLength;

public:
    int width;
    int height;
    int originX;
    int originY;

    ImageAnimation(std::string path, bool swapPalette);

    ~ImageAnimation();

    SDL_Texture* getTexture(int frame);
};
