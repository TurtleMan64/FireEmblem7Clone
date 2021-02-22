#pragma once

#include <string>

#include "Unit.hpp"

enum MapTileType
{
    Normal   = '0',
    Wall     = '1', //flying units can go on
    Forest   = '2', //-1 mov, +1 def +20 avoid
    Mountain = '3', //-2 mov, +2 def +30 avoid
    Peak     = '4', //-3 mov, +3 def +40 avoid
    Water    = '5', //-2 mov, +20 avoid
    NoPass   = '6', //unpassable by everyone
    Sand     = '7', //-1 mov
    Armory   = 'A',
    Break    = 'B',
    Chest    = 'C',
    Door     = 'D',
    Fort     = 'F',
    Throne   = 'T',
    House    = 'H',
    Village  = 'V',
    Shop     = 'S',
};

class MapTile
{
public:
    //maybe have Sprite defined for each tile? going to need to do this in order for the map to change
    MapTileType type;
    std::string name;
    int avoid;
    int defense;

    MapTile(MapTileType type, std::string name);

    int movementLostForMovingOnThisTile(Unit* unit);
};
