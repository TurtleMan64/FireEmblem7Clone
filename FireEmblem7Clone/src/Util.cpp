#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>

#include "Global.hpp"
#include "Util.hpp"
#include "Unit.hpp"

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
            else if (r == 144 && g == 184 && b == 232)
            {
                r = 192;
                g = 168;
                b = 184;
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

int Util::random2()
{
    return (random() + random())/2;
}

int Util::random()
{
    return (int)(100*(((float)rand())/(RAND_MAX + 1)));
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

int Util::mod(int val, int mod)
{
    val = val % mod;
    if (val < 0)
    {
        val += mod;
    }
    return val;
}

int Util::getManhattanDistance(Unit* unit1, Unit* unit2)
{
    int diffX = unit1->tileX - unit2->tileX;
    int diffY = unit1->tileY - unit2->tileY;

    return abs(diffX) + abs(diffY);
}

int Util::getWeaponTriangle(WeaponStats myWeapon, WeaponStats otherWeapon)
{
    if (myWeapon.type == NotWeapon || otherWeapon.type == NotWeapon ||
        myWeapon.type == Bow       || otherWeapon.type == Bow       ||
        myWeapon.type == Staff     || otherWeapon.type == Staff)
    {
        return 0;
    }

    switch (myWeapon.type)
    {
        case Sword:
            if (otherWeapon.type == Lance) return -1;
            if (otherWeapon.type == Axe)   return  1;
            return 0;

        case Lance:
            if (otherWeapon.type == Axe)   return -1;
            if (otherWeapon.type == Sword) return  1;
            return 0;

        case Axe:
            if (otherWeapon.type == Sword) return -1;
            if (otherWeapon.type == Lance) return  1;
            return 0;

        case Anima:
            if (otherWeapon.type == Dark)  return -1;
            if (otherWeapon.type == Light) return  1;
            return 0;

        case Light:
            if (otherWeapon.type == Anima) return -1;
            if (otherWeapon.type == Dark)  return  1;
            return 0;

        case Dark:
            if (otherWeapon.type == Light) return -1;
            if (otherWeapon.type == Anima) return  1;
            return 0;

        default:
            return 0;
    }
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

int Util::getIndex(std::vector<Unit*>* vector, Unit* e)
{
    for (int i = 0; i < vector->size(); i++)
    {
        if (vector->at(i) == e)
        {
            return i;
        }
    }

    return -1;
}
