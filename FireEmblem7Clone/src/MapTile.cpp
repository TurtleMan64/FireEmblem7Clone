#include "MapTile.hpp"
#include "ClassData.hpp"

int MapTile::movementCosts[32][11];

void MapTile::init()
{
    int N = 50;

    int data[32][11] =
    {
        { N, N, N, N, N, N, N, N, 1, N, N },
        { N, N, N, N, N, N, N, N, 1, N, N },
        { N, N, N, N, N, N, N, N, 1, N, N },
        { N, N, N, N, N, N, N, N, 1, N, N },
        { N, N, N, N, N, N, N, N, N, N, N },
        { N, N, N, N, N, N, N, N, N, N, N },
        { N, N, N, N, N, N, N, N, N, N, N },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2 },
        { 5, N, N, N, 5, N, N, N, 1, 5, 2 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 2, 3, 2, 2, 3, 3, 3, 1, 1, 2, 2 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { N, N, N, N, N, N, N, N, 1, N, 2 },
        { N, N, N, N, N, N, N, N, 1, N, 3 },
        { 2, 2, 3, 3, 2, 2, 2, 2, 1, 2, 2 },
        { 2, 2, 3, 3, 3, 3, 2, 2, 1, 2, 2 },
        { 4, N, N, 6, N, 5, 3, 4, 1, 3, 3 },
        { N, N, N, N, N, N, N, N, 1, 4, N },
        { 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { N, N, N, N, N, N, N, N, 1, N, N }
    };

    for (int x = 0; x < 32; x++)
    {
        for (int y = 0; y < 11; y++)
        {
            MapTile::movementCosts[x][y] = data[x][y];
        }
    }
}

MapTile::MapTile(MapTileType type)
{
    this->type = type;

    defense = 0;
    avoid = 0;
    name = "--";

    switch (type)
    {
        case NoPass:
            name = "--";
            break;

        case Cliff:
            name = "Cliff";
            break;

        case Fence:
            name = "Fence";
            break;

        case Snag:
            name = "Snag";
            break;

        case Wall:
            name = "Wall";
            break;

        case Door:
            name = "Door";
            break;

        case WallCrack:
            name = "Wall";
            break;

        case Plain:
            name = "Plain";
            break;

        case Road:
            name = "Road";
            break;

        case Floor:
            name = "Floor";
            break;

        case Stairs:
            name = "Stairs";
            break;

        case Deck:
            name = "Deck";
            break;

        case Bridge:
            name = "Bridge";
            break;

        case RuinsVillage:
            name = "Ruins";
            break;

        case River:
            name = "River";
            break;

        case Sand:
            name = "Sand";
            avoid = 5;
            break;

        case Desert:
            name = "Desert";
            avoid = 5;
            break;

        case House:
            name = "House";
            avoid = 10;
            break;

        case Shop:
            name = "Shop";
            avoid = 10;
            break;

        case Arena:
            name = "Arena";
            avoid = 10;
            break;

        case Village:
            name = "Village";
            avoid = 10;
            break;

        case Ruins:
            name = "Ruins";
            avoid = 10;
            break;

        case Sea:
            name = "Sea";
            avoid = 10;
            break;

        case Lake:
            name = "Lake";
            avoid = 10;
            break;

        case Forest:
            name = "Forest";
            avoid = 20;
            defense = 1;
            break;

        case Pillar:
            name = "Pillar";
            avoid = 20;
            defense = 1;
            break;

        case Mountain:
            name = "Mountain";
            avoid = 30;
            defense = 1;
            break;

        case Peak:
            name = "Peak";
            avoid = 40;
            defense = 2;
            break;

        case Fort:
            name = "Fort";
            avoid = 20;
            defense = 2;
            break;

        case Gate:
            name = "Gate";
            avoid = 20;
            defense = 3;
            break;

        case Throne:
            name = "Throne";
            avoid = 20;
            defense = 2;
            break;

        case Void:
            name = "--";
            break;

        default:
            break;
    }
}

// https://serenesforest.net/blazing-sword/classes/terrain-data/
int MapTile::movementLostForMovingOnThisTile(Unit* unit)
{
    ClassGroup group = Foot;

    switch (unit->classResources.classType)
    {
        case ClassType::LordCaelin:
        case ClassType::LordOstia:
        case ClassType::LordPherae:
        case ClassType::BladeLord:
        case ClassType::SwordmasterF:
        case ClassType::SwordmasterM:
        case ClassType::HeroF:
        case ClassType::HeroM:
        case ClassType::SniperF:
        case ClassType::SniperM:
        case ClassType::ThiefF:
        case ClassType::ThiefM:
        case ClassType::Assassin:
            group = Foot;
            break;

        case ClassType::GreatLord:
        case ClassType::Knight:
        case ClassType::General:
            group = Armours;
            break;

        case ClassType::Cavalier:
        case ClassType::Troubadour:
            group = Knights1;
            break;

        case ClassType::KnightLord:
        case ClassType::Paladin:
        case ClassType::Valkyrie:
            group = Knights2;
            break;

        case ClassType::NomadF:
        case ClassType::NomadM:
            group = Nomads;
            break;

        case ClassType::NomadTrooperF:
        case ClassType::NomadTrooperM:
            group = NomadTroopers;
            break;

        case ClassType::Fighter:
        case ClassType::Warrior:
            group = Fighters;
            break;

        case ClassType::Brigand:
        case ClassType::Berserker:
            group = Bandits;
            break;

        case ClassType::Pirate:
        case ClassType::Corsair:
        //case ClassType::Berserker: Berserker is also in Bandits group??
            group = Pirates;
            break;

        case ClassType::MageF:
        case ClassType::MageM:
        case ClassType::SageF:
        case ClassType::SageM:
        case ClassType::Archsage:
        case ClassType::ShamanF:
        case ClassType::ShamanM:
        case ClassType::DruidF:
        case ClassType::DruidM:
        case ClassType::DarkDruid:
        case ClassType::Cleric:
        case ClassType::Monk:
        case ClassType::BishopF:
        case ClassType::BishopM:
        case ClassType::MagicSeal:
            group = Mages;
            break;

        case ClassType::PegasusKnight:
        case ClassType::FalconKnight:
        case ClassType::WyvernRider:
        case ClassType::WyvernLord:
            group = Fliers;
            break;

        default:
            group = Foot;
            break;
    }

    int penalty = movementCosts[type][group];

    return penalty;
}
