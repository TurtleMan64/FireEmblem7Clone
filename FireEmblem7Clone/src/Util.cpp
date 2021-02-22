#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <string>
#include <vector>
#include <fstream>

#include "Global.hpp"
#include "Util.hpp"

SDL_Texture* Util::loadRedImage(std::string path)
{
    SDL_Surface* surf = IMG_Load(path.c_str());
    if (surf == nullptr)
    {
        printf("Error: Could not open file '%s'\n", path.c_str());
    }
    Uint32* pixels = (Uint32*)surf->pixels;

    for (int y = 0; y < surf->h; y++)
    {
        for (int x = 0; x < surf->w; x++)
        {
            Uint32 pixel = pixels[x + y*surf->w];

            Uint32 r = pixel & 0x000000FF;
            Uint32 g = pixel & 0x0000FF00;
            Uint32 b = pixel & 0x00FF0000;
            Uint32 a = pixel & 0xFF000000;

            g = g >>  8;
            b = b >> 16;
            a = a >> 24;


            if (r == 56 && g == 56 && b == 144)
            {
                r = 96;
                g = 40;
                b = 32;
            }
            else if (r == 56 && g == 80 && b == 224)
            {
                r = 168;
                g = 48;
                b = 40;
            }
            else if (r == 40 && g == 160 && b == 248)
            {
                r = 224;
                g = 16;
                b = 16;
            }
            else if (r == 24 && g == 240 && b == 248)
            {
                r = 248;
                g = 80;
                b = 72;
            }
            else if (r == 232 && g == 16 && b == 24)
            {
                r = 56;
                g = 208;
                b = 48;
            }

            g = g <<  8;
            b = b << 16;
            a = a << 24;

            pixel = a | r | g | b;
            pixels[x + y*surf->w] = pixel;
        }
    }

    SDL_Texture* t = SDL_CreateTextureFromSurface(Global::sdlRenderer, surf);
    SDL_FreeSurface(surf);
    return t;
}

int Util::clamp(int lower, int var, int higher)
{
    if (var < lower)  return lower;
    if (var > higher) return higher;
    return var;
}

int Util::approach(int current, int target, float rate)
{
    if (current != target)
    {
        float toMove = (target - current)*rate;
        if (fabsf(toMove) < 1.0f)
        {
            if (toMove > 0.0f)
            {
                toMove = 1.0f;
            }
            else
            {
                toMove = -1.0f;
            }
        }
        current += (int)toMove;
    }

    return current;
}

bool Util::fileExists(std::string path)
{
    std::ifstream file(path);
    return file.is_open();
}

std::vector<std::string> Util::readFile(std::string path)
{
    std::vector<std::string> lines;

    std::ifstream file(path);
    if (!file.is_open())
    {
        printf("Error: Could not open file '%s'\n", path.c_str());
        return lines;
    }

    std::string line;

    while (!file.eof())
    {
        std::getline(file, line);
        if (!file.eof())
        {
            lines.push_back(line);
        }
    }
    
    return lines;
}

std::vector<std::string> Util::split(std::string line, char delim)
{
    if (delim == 0)
    {
        std::vector<std::string> list;
        printf("Cannot split a string on null character.\n");
        return list;
    }

    if (line.size() >= 1023)
    {
        std::vector<std::string> list;
        printf("string sent to split() is too long.\n");
        return list;
    }

    char lineBuf[1024];
    memset(lineBuf, 0, 1024);
    memcpy(lineBuf, line.c_str(), line.size());

    // Scan through line to find the number of tokens
    int numTokens = 0;
    int index = 0;
    int inToken = 0;

    while (line[index] != 0)
    {
        if (line[index] != delim && inToken == 0)
        {
            inToken = 1;
            numTokens += 1;
        }
        else if (line[index] == delim)
        {
            inToken = 0;
        }
        index += 1;
    }

    // Get memory to store the data
    char** parsedData = (char**)malloc(sizeof(char*)*(numTokens + 1));

    // Scan through line to fill parsedData
    //  and set null characters after tokens
    index = 0;
    inToken = 0;
    int tokenNum = 0;

    while (line[index] != 0)
    {
        if (line[index] != delim && inToken == 0)
        {
            parsedData[tokenNum] = &line[index];
            tokenNum += 1;
            inToken = 1;
        }
        else if (line[index] == delim)
        {
            if (inToken == 1)
            {
                line[index] = 0;
            }
            inToken = 0;
        }
        index += 1;
    }

    parsedData[numTokens] = nullptr;

    std::vector<std::string> dat;

    for (int i = 0; i < numTokens; i++)
    {
        dat.push_back(parsedData[i]);
    }

    free(parsedData);

    return dat;
}
