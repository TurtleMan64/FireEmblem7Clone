#pragma once

#include <string>
#include <unordered_map>

enum ClassType
{
    LordCaelin,
    BladeLord,
    LordPherae,
    KnightLord,
    LordOstia,
    GreatLord,
    Mercenary,
    Hero,
    Myrmidon,
    Swordmaster,
    Thief,
    Assassin,
    Knight,
    General,
    Soldier,
    Fighter,
    Warrior,
    Brigand,
    Pirate,
    Corsair,
    Berserker,
    Archer,
    Sniper,
    Nomad,
    NomadTrooper,
    Cavalier,
    Paladin,
    PegasusKnight,
    FalconKnight,
    WyvernRider,
    WyvernLord,
    Monk,
    Cleric,
    Bishop,
    Troubadour,
    Valkyrie,
    Mage,
    Sage,
    Archsage,
    Shaman,
    Druid,
    DarkDruid,
    Bard,
    Dancer,
    MagicSeal,
    Bramimond,
    FireDragon,
    Civilian,
    Prince,
    TransporterTent,
    TransporterWagon
};

class ClassData
{
private:
    static std::unordered_map<std::string, ClassType> nameToClass;

public:
    class StatCaps
    {
    public:
        int hp;
        int str;
        int mag;
        int skl;
        int spd;
        int lck;
        int def;
        int res;
    };

    static void init();

    static std::string getClassName(ClassType type);

    static ClassType getClassType(std::string name);
};
