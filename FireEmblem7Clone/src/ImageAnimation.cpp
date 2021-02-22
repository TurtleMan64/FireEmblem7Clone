#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <string>
#include <vector>
#include <unordered_map>

#include "Global.hpp"
#include "Util.hpp"
#include "ImageAnimation.hpp"

ImageAnimation::ImageAnimation(std::string path, bool swapPalette)
{
    std::string animFilePath = path + "/anim.ini";
    std::vector<std::string> animFile = Util::readFile(animFilePath);

    int numImages = std::stoi(animFile[0]);
    width         = std::stoi(animFile[1]);
    height        = std::stoi(animFile[2]);
    originX       = std::stoi(animFile[3]);
    originY       = std::stoi(animFile[4]);
    int red       = std::stoi(animFile[5]);
    int green     = std::stoi(animFile[6]);
    int blue      = std::stoi(animFile[7]);
    int alpha     = std::stoi(animFile[8]);

    totalAnimationLength = 0;
    for (int i = 0; i < numImages; i++)
    {
        std::string imgPath = path + "/" + std::to_string(i) + ".png";

        SDL_Texture* tex = nullptr;
        if (!swapPalette)
        {
            tex = IMG_LoadTexture(Global::sdlRenderer, imgPath.c_str());
            if (tex == nullptr)
            {
                printf("Error: Could not open file '%s'\n", imgPath.c_str());
            }
        }
        else
        {
            tex = Util::loadRedImage(imgPath);
        }

        SDL_SetTextureColorMod(tex, (Uint8)red, (Uint8)green, (Uint8)blue);
        SDL_SetTextureAlphaMod(tex, (Uint8)alpha);

        images.push_back(tex);

        int framesForThisImage = std::stoi(animFile[i + 10]);
        for (int c = 0; c < framesForThisImage; c++)
        {
            frameToTexture[totalAnimationLength] = i;
            totalAnimationLength++;
        }
    }
}

ImageAnimation::~ImageAnimation()
{
    for (int i = 0; i < images.size(); i++)
    {
        SDL_DestroyTexture(images[i]);
    }

    images.clear();
}

SDL_Texture* ImageAnimation::getTexture(int frame)
{
    if (frame < 0)
    {
        frame = 0;
    }

    frame = frame % totalAnimationLength;

    return images[frameToTexture[frame]];
}
