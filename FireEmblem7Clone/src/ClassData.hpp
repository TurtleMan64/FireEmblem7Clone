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
    HeroF,
    HeroM,
    MyrmidonF,
    MyrmidonM,
    SwordmasterF,
    SwordmasterM,
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
    ArcherF,
    ArcherM,
    SniperF,
    SniperM,
    NomadF,
    NomadM,
    NomadTrooperF,
    NomadTrooperM,
    Cavalier,
    Paladin,
    PegasusKnight,
    FalconKnight,
    WyvernRider,
    WyvernLord,
    Monk,
    Cleric,
    BishopF,
    BishopM,
    Troubadour,
    Valkyrie,
    MageF,
    MageM,
    SageF,
    SageM,
    Archsage,
    ShamanF,
    ShamanM,
    DruidF,
    DruidM,
    DarkDruid,
    Bard,
    Dancer,
    MagicSeal,
    Bramimond,
    FireDragon,
    Man,
    Prince,
    TransporterTent,
    TransporterWagon
};

class ClassResources
{
public:
    ClassType classType;
    std::string displayName;

    std::string mapSpritesPath;
    std::string battleSpritesPath;
    std::string genericMugshotPath;
    std::string genericMugshotTinyPath;

    int capHp;
    int capStr;
    int capMag;
    int capSkl;
    int capSpd;
    int capLck;
    int capDef;
    int capRes;
    int capCon;
    int capMov;

    int promoHp;
    int promoStr;
    int promoMag;
    int promoSkl;
    int promoSpd;
    int promoLck;
    int promoDef;
    int promoRes;
    int promoCon;
    int promoMov;

    int promoSword;
    int promoLance;
    int promoAxe;
    int promoBow;
    int promoAnima;
    int promoLight;
    int promoDark;
    int promoStaff;

    ClassResources();
    ClassResources(std::string internalName, ClassType type);
};

class ClassData
{
private:
    static std::unordered_map<std::string, ClassResources> nameToClassResources;

public:
    static void init();

    static ClassResources getClassResources(std::string internalName);
};
