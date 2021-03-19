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

    //returns average of 2 random calls [0, 99] 
    static int random2();

    //returns random [0, 99]
    static int random();

    static int approach(int current, int target, float rate);

    //for positive values, same as val % mod.
    // for negative val, if val is still negative after %, then adds mod.
    // so mod( -3, 10) -> 7
    //    mod(-13, 10) -> 7
    static int mod(int val, int mod);

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
