#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <unordered_set>

#include "Weapons.hpp"

enum ItemId
{
    None         =   0,

    IronSword    =   1,
    SlimSword    =   2,
    EmblemBlade  =   3,
    PoisonSword  =   4,
    SteelSword   =   5,
    IronBlade    =   6,
    Armorslayer  =   7,
    Longsword    =   8,
    WoDao        =   9,
    SteelBlade   =  10,
    KillingEdge  =  11,
    Wyrmslayer   =  12,
    LightBlade   =  13,
    Lancereaver  =  14,
    BraveSword   =  15,
    WindSword    =  16,
    SilverSword  =  17,
    SilverBlade  =  18,
    RuneSword    =  19,
    RegalBlade   =  20,
    ManiKatti    =  21,
    Rapier       =  22,
    Durandal     =  23,
    SolKatti     =  24,

    IronLance    =  25,
    SlimLance    =  26,
    Javelin      =  27,
    EmblemLance  =  28,
    PoisonLance  =  29,
    SteelLance   =  30,
    HeavySpear   =  31,
    Horseslayer  =  32,
    ShortSpear   =  33,
    KillerLance  =  34,
    Axereaver    =  35,
    BraveLance   =  36,
    Spear        =  37,
    Spear2       =  38,
    SilverLance  =  39,
    RexHasta     =  40,

    HandAxe      =  41,
    IronAxe      =  42,
    EmblemAxe    =  43,
    SteelAxe     =  44,
    DevilAxe     =  45,
    PoisonAxe    =  46,
    Halberd      =  47,
    Hammer       =  48,
    DragonAxe    =  49,
    KillerAxe    =  50,
    Swordreaver  =  51,
    Swordslayer  =  52,
    BraveAxe     =  53,
    Tomahawk     =  54,
    SilverAxe    =  55,
    Basilikos    =  56,
    WolfBeil     =  57,
    Armads       =  58,

    IronBow      =  59,
    EmblemBow    =  60,
    PoisonBow    =  61,
    ShortBow     =  62,
    Longbow      =  63,
    SteelBow     =  64,
    KillerBow    =  65,
    BraveBow     =  66,
    SilverBow    =  67,
    Rienfleche   =  68,

    Fire         =  69,
    Thunder      =  70,
    Elfire       =  71,
    Bolting      =  72,
    Fimbulvetr   =  73,
    Excalibur    =  74,
    Forblaze     =  75,

    Lightning    =  76,
    Shine        =  77,
    Divine       =  78,
    Purge        =  79,
    Aura         =  80,
    Aureola      =  81,
    Luce         =  82,

    Flux         =  83,
    Luna         =  84,
    Nosferatu    =  85,
    Eclipse      =  86,
    Fenrir       =  87,
    Gespenst     =  88,
    Ereshkigal   =  89,

    Heal         =  90,
    Mend         =  91,
    TorchStaff   =  92,
    Unlock       =  93,
    Recover      =  94,
    Restore      =  95,
    Hammerne     =  96,
    Barrier      =  97,
    Physic       =  98,
    Silence      =  99,
    Sleep        = 100,
    Berserk      = 101,
    Rescue       = 102,
    Fortify      = 103,
    Warp         = 104,

    DelphiShield = 105,
    IronRune     = 106,
    FilasMight   = 107,
    NinisGrace   = 108,
    ThorsIre     = 109,
    SetsLitany   = 110,
    AfasDrops    = 111,
    DoorKey      = 112,
    ChestKey     = 113,
    ChestKey5    = 114,
    Vulnerary    = 115,
    Antitoxin    = 116,
    Torch        = 117,
    Mine         = 118,
    LightRune    = 119,
    PureWater    = 120,
    Lockpick     = 121,
    Elixir       = 122,
    SilverCard   = 123,
    MemberCard   = 124,
    RedGem       = 125,
    BlueGem      = 126,
    WhiteGem     = 127,
    AngelicRobe  = 128,
    EnergyRing   = 129,
    SecretBook   = 130,
    Speedwing    = 131,
    GoddessIcon  = 132,
    Dracoshield  = 133,
    Talisman     = 134,
    Boots        = 135,
    BodyRing     = 136,
    HeroCrest    = 137,
    KnightCrest  = 138,
    OrionsBolt   = 139,
    ElysianWhip  = 140,
    GuidingRing  = 141,
    EarthSeal    = 142,
    HeavensSeal  = 143,
    FellContract = 144,
    OceanSeal    = 145,
    EmblemSeal   = 146,
    Flametongue  = 147
};

class Unit;

class Item
{
private:
    static SDL_Texture* itemsTex;

public:
    ItemId id = None;
    int usesRemaining = 1;

    Item(ItemId id);

    void render(int x, int y);

    bool isWeapon();

    bool isConsumableByUnit(Unit* unit);

    void consume(Unit* unit);

    std::unordered_set<int> getWeaponRange();

    WeaponStats getWeaponStats();

    std::string getName();
};
