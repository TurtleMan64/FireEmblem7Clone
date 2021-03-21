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

    bool isFlier = (unit->classResources.classType == ClassType::PegasusKnight) ||
                   (unit->classResources.classType == ClassType::FalconKnight)  ||
                   (unit->classResources.classType == ClassType::WyvernRider)   ||
                   (unit->classResources.classType == ClassType::WyvernLord);

    bool isHorse = (unit->classResources.classType == ClassType::KnightLord)    ||
                   (unit->classResources.classType == ClassType::Cavalier)      ||
                   (unit->classResources.classType == ClassType::Paladin)       ||
                   (unit->classResources.classType == ClassType::Troubadour)    ||
                   (unit->classResources.classType == ClassType::Valkyrie)      ||
                   (unit->classResources.classType == ClassType::NomadF)        ||
                   (unit->classResources.classType == ClassType::NomadM)        ||
                   (unit->classResources.classType == ClassType::NomadTrooperF) ||
                   (unit->classResources.classType == ClassType::NomadTrooperM);

    bool isMagic = (unit->classResources.classType == ClassType::MageF)     ||
                   (unit->classResources.classType == ClassType::MageM)     ||
                   (unit->classResources.classType == ClassType::SageF)     ||
                   (unit->classResources.classType == ClassType::SageM)     ||
                   (unit->classResources.classType == ClassType::Cleric)    ||
                   (unit->classResources.classType == ClassType::Monk)      ||
                   (unit->classResources.classType == ClassType::BishopF)   ||
                   (unit->classResources.classType == ClassType::BishopM)   ||
                   (unit->classResources.classType == ClassType::ShamanF)   ||
                   (unit->classResources.classType == ClassType::ShamanM)   ||
                   (unit->classResources.classType == ClassType::DruidF)    ||
                   (unit->classResources.classType == ClassType::DruidM)    ||
                   (unit->classResources.classType == ClassType::DarkDruid) ||
                   (unit->classResources.classType == ClassType::Archsage)  ||
                   (unit->classResources.classType == ClassType::MagicSeal) ||
                   (unit->classResources.classType == ClassType::Bramimond);

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
