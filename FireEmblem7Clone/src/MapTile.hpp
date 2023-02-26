#pragma once

#include <string>

#include "Unit.hpp"

enum MapTileType
{
    NoPass       =  0, // Spot that is "out of bounds". No unit can enter it under any circumstance.
    Cliff        =  1,
    Fence        =  2,
    Snag         =  3,
    Wall         =  4,
    Door         =  5,
    WallCrack    =  6,
    Plain        =  7,
    Road         =  8,
    Floor        =  9,
    Stairs       = 10,
    Deck         = 11,
    Bridge       = 12,
    RuinsVillage = 13,
    River        = 14,
    Sand         = 15,
    Desert       = 16,
    House        = 17,
    Shop         = 18,
    Arena        = 19,
    Village      = 20,
    Ruins        = 21,
    Sea          = 22,
    Lake         = 23,
    Forest       = 24,
    Pillar       = 25,
    Mountain     = 26,
    Peak         = 27,
    Fort         = 28,
    Gate         = 29,
    Throne       = 30,
    Void         = 31, // Like NoPass, except fliers can go over it.
};

class MapTile
{
private:
    enum ClassGroup
    {
        Foot,
        Armours,
        Knights1,
        Knights2,
        Nomads,
        NomadTroopers,
        Fighters,
        Mages,
        Fliers,
        Bandits,
        Pirates
    };

    static int movementCosts[32][11];

public:
    //maybe have Sprite defined for each tile? going to need to do this in order for the map to change
    MapTileType type;
    std::string name;
    int avoid;
    int defense;

    MapTile(MapTileType type);

    int movementLostForMovingOnThisTile(Unit* unit);

    static void init();
};
