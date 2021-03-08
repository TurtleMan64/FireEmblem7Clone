#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <string>
#include <vector>

#include "Weapons.hpp"
#include "Unit.hpp"

class Util
{
public:
    //makes var be in [lower, higher] range
    static int clamp(int lower, int var, int higher);

    static int approach(int current, int target, float rate);

    static int getManhattanDistance(Unit* unit1, Unit* unit2);

    //-1 if myWeapon is bad against other, 0 if neutral, 1 if myWeapon is good against other
    static int getWeaponTriangle(WeaponStats myWeapon, WeaponStats otherWeapon);

    static SDL_Texture* loadRedImage(std::string path);

    static bool fileExists(std::string path);

    static std::vector<std::string> readFile(std::string path);

    //given a string, split the string into a vector of strings
    // based on some delimiter character. the string canont be more 
    // than 1023 characters long.
    static std::vector<std::string> split(std::string line, char delim);
};
