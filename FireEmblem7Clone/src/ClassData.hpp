#pragma once

#include <unordered_set>

enum ClassType
{
    Lord,
    Cavalier,
    Pegasus,
    Archer,
    Cleric,
    Fighter,
    Myrmidon,
    Troubadour,
    Mercenary,
    Shaman,
    Pirate,
    Thief,
    Dancer,
    Wyvern,
    Mage,
    Nomad,
    Monk,
    Knight,
    Brigand
};

class ClassData
{
public:
    class StatCaps
    {
    public:
        int healthPoints;
        int strength;
        int skill;
        int speed;
        int luck;
        int defense;
        int resistance;
    };

    //class Wea

    static void loadClassData();
};
