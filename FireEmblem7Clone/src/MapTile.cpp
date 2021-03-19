#include "MapTile.hpp"
#include "ClassData.hpp"

MapTile::MapTile(MapTileType type, std::string name)
{
    this->type = type;
    this->name = name;

    defense = 0;
    avoid = 0;
    this->name = "--";

    switch (type)
    {
        case Normal:
            this->name = "Plain";
            break;

        case Wall:
            this->name = "--";
            break;

        case Forest:
            this->name = "Forest";
            defense = 1;
            avoid = 20;
            break;

        case Mountain:
            this->name = "Mntn";
            defense = 1;
            avoid = 30;
            break;

        case Peak:
            this->name = "Peak";
            defense = 2;
            avoid = 40;
            break;

        case Water:
            this->name = "Water";
            avoid = 20;
            break;

        case Sand:
            this->name = "Sand";
            avoid = 5;
            break;

        case Fort:
            this->name = "Fort";
            defense = 2;
            avoid = 20;
            break;

        case Armory:
            this->name = "Armory";
            avoid = 10;
            break;

        case House:
            this->name = "House";
            avoid = 10;
            break;

        case Shop:
            this->name = "Vendor";
            avoid = 10;
            break;

        case Village:
            this->name = "Village";
            avoid = 10;
            break;

        case Throne:
            this->name = "Throne";
            defense = 3;
            avoid = 20;
            break;

        default:
            break;
    }
}

int MapTile::movementLostForMovingOnThisTile(Unit* unit)
{
    const int NOPE = 100;

    bool isFlier = (unit->classType == ClassType::PegasusKnight) ||
                   (unit->classType == ClassType::FalconKnight)  ||
                   (unit->classType == ClassType::WyvernRider)   ||
                   (unit->classType == ClassType::WyvernLord);

    bool isHorse = (unit->classType == ClassType::KnightLord) ||
                   (unit->classType == ClassType::Cavalier)   ||
                   (unit->classType == ClassType::Paladin)    ||
                   (unit->classType == ClassType::Troubadour) ||
                   (unit->classType == ClassType::Valkyrie)   ||
                   (unit->classType == ClassType::Nomad)      ||
                   (unit->classType == ClassType::NomadTrooper);

    bool isMagic = (unit->classType == ClassType::Mage)      ||
                   (unit->classType == ClassType::Sage)      ||
                   (unit->classType == ClassType::Cleric)    ||
                   (unit->classType == ClassType::Monk)      ||
                   (unit->classType == ClassType::Bishop)    ||
                   (unit->classType == ClassType::Shaman)    ||
                   (unit->classType == ClassType::Druid)     ||
                   (unit->classType == ClassType::DarkDruid) ||
                   (unit->classType == ClassType::Archsage)  ||
                   (unit->classType == ClassType::MagicSeal) ||
                   (unit->classType == ClassType::Bramimond);

    switch (type)
    {
        case Wall:
            if (isFlier) return 1;
            return NOPE;

        case Forest:
            if (isFlier) return 1;
            if (isHorse) return 3;
            return 2;

        case Mountain:
            if (isFlier) return 1;
            if (isHorse) return 5;
            return 3;

        case Peak:
            if (isFlier) return 1;
            if (isHorse) return NOPE;
            return 4;

        case Water:
            if (isFlier) return 1;
            if (isHorse) return NOPE;
            return 3;

        case Sand:
            if (isFlier) return 1;
            if (isHorse) return 3;
            if (isMagic) return 1;
            return 2;

        case NoPass:
            return NOPE;

        default:
            return 1;
    }
}
