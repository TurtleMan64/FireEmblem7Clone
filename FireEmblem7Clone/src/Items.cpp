#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <unordered_set>

#include "Weapons.hpp"
#include "Unit.hpp"
#include "Global.hpp"
#include "Items.hpp"


SDL_Texture* Item::itemsTex = nullptr;

Item::Item(ItemId id)
{
    this->id = id;

    if (itemsTex == nullptr)
    {
        itemsTex = IMG_LoadTexture(Global::sdlRenderer, "res/Images/Items/Items.png");
        if (itemsTex == nullptr)
        {
            printf("Error: could not load items texture\n");   
        }
    }
}

void Item::render(int x, int y)
{
    int idx = (int)id;
    SDL_Rect src;
    src.x = idx*16;
    src.y = 0;
    src.w = 16;
    src.h = 16;

    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = 16;
    dst.h = 16;

    SDL_RenderCopy(Global::sdlRenderer, itemsTex, &src, &dst);
}

bool Item::isWeapon()
{
    int idx = (int)id;
    if (idx > 0 && idx < (int)Heal)
    {
        return true;
    }

    if (id == Flametongue)
    {
        return true;
    }

    return false;
}

bool Item::isConsumableByUnit(Unit* unit)
{
    switch (id)
    {
        case AfasDrops    : return true;

        case Vulnerary :
        case Elixir    :
        {
            return (unit->hp < unit->maxHp);
        }

        case Antitoxin    : return true;
        case Torch        : return true;
        case Mine         : return true;
        case LightRune    : return true;
        case PureWater    : return true;


        case AngelicRobe  : return true;
        case EnergyRing   : return true;
        case SecretBook   : return true;
        case Speedwing    : return true;
        case GoddessIcon  : return true;
        case Dracoshield  : return true;
        case Talisman     : return true;
        case Boots        : return true;
        case BodyRing     : return true;
        case HeroCrest    : return true;
        case KnightCrest  : return true;
        case OrionsBolt   : return true;
        case ElysianWhip  : return true;
        case GuidingRing  : return true;
        case EarthSeal    : return true;
        case HeavensSeal  : return true;
        case FellContract : return true;
        case OceanSeal    : return true;
        case EmblemSeal   : return true;

        default:            return false;
    }
}

void Item::consume(Unit* unit)
{
    switch (id)
    {
        case AfasDrops:
        {
            unit->growthHp  += 5;
            unit->growthStr += 5;
            unit->growthMag += 5;
            unit->growthSkl += 5;
            unit->growthSpd += 5;
            unit->growthLck += 5;
            unit->growthDef += 5;
            unit->growthRes += 5;
            return;
        }

        case Vulnerary:
        {
            unit->hp += 10;
            if (unit->hp > unit->maxHp)
            {
                unit->hp = unit->maxHp;
            }
            return;
        }

        case Elixir:
        {
            unit->hp = unit->maxHp;
            return;
        }

        //case Antitoxin    : return true;
        //case Torch        : return true;
        //case Mine         : return true;
        //case LightRune    : return true;
        //case PureWater    : return true;
        //
        //
        //case AngelicRobe  : return true;
        //case EnergyRing   : return true;
        //case SecretBook   : return true;
        //case Speedwing    : return true;
        //case GoddessIcon  : return true;
        //case Dracoshield  : return true;
        //case Talisman     : return true;
        //case Boots        : return true;
        //case BodyRing     : return true;
        //case HeroCrest    : return true;
        //case KnightCrest  : return true;
        //case OrionsBolt   : return true;
        //case ElysianWhip  : return true;
        //case GuidingRing  : return true;
        //case EarthSeal    : return true;
        //case HeavensSeal  : return true;
        //case FellContract : return true;
        //case OceanSeal    : return true;
        //case EmblemSeal   : return true;

        //default:            return false;
        default: return;
    }
}

std::unordered_set<int> Item::getWeaponRange()
{
    std::unordered_set<int> ranges;

    if (!isWeapon())
    {
        return ranges;
    }
    WeaponStats stats = getWeaponStats();

    switch (stats.type)
    {
        case Sword:
            ranges.insert(1);
            if (id == LightBlade ||
                id == WindSword  ||
                id == RuneSword)
            {
                ranges.insert(2);
            }
            return ranges;

        case Lance:
            ranges.insert(1);
            if (id == Javelin    ||
                id == ShortSpear ||
                id == Spear      ||
                id == Spear2)
            {
                ranges.insert(2);
            }
            return ranges;

        case Axe:
            ranges.insert(1);
            if (id == HandAxe ||
                id == Tomahawk)
            {
                ranges.insert(2);
            }
            return ranges;
        
        case Bow:
            ranges.insert(2);
            return ranges;

        case Anima:
            if (id == Bolting)
            {
                ranges.insert(3);
                ranges.insert(4);
                ranges.insert(5);
                ranges.insert(6);
                ranges.insert(7);
                ranges.insert(8);
                ranges.insert(9);
                ranges.insert(10);
            }
            else
            {
                ranges.insert(1);
                ranges.insert(2);
            }
            return ranges;

        case Light:
            if (id == Purge)
            {
                ranges.insert(3);
                ranges.insert(4);
                ranges.insert(5);
                ranges.insert(6);
                ranges.insert(7);
                ranges.insert(8);
                ranges.insert(9);
                ranges.insert(10);
            }
            else
            {
                ranges.insert(1);
                ranges.insert(2);
            }
            return ranges;

        case Dark:
            if (id == Fenrir)
            {
                ranges.insert(3);
                ranges.insert(4);
                ranges.insert(5);
                ranges.insert(6);
                ranges.insert(7);
                ranges.insert(8);
                ranges.insert(9);
                ranges.insert(10);
            }
            else
            {
                ranges.insert(1);
                ranges.insert(2);
            }
            return ranges;

        default:
            if (id == Flametongue)
            {
                ranges.insert(3);
            }
            return ranges;
    }
}

WeaponStats Item::getWeaponStats()
{
    switch (id)
    {
        case None         : return {NotWeapon, N,  1,  1,  1,   1,  0, 1,     1};

        case IronSword    : return {Sword,     E, 46,  5,  5,  90,  0, 1,   460};
        case SlimSword    : return {Sword,     E, 30,  2,  3, 100,  5, 1,   480};
        case EmblemBlade  : return {Sword,     E, 60,  5,  5,  90,  0, 1,    -1};
        case PoisonSword  : return {Sword,     D, 40,  6,  3,  70,  0, 1,    -1};
        case SteelSword   : return {Sword,     D, 30, 10,  8,  75,  0, 1,   600};
        case IronBlade    : return {Sword,     D, 35, 12,  9,  70,  0, 2,   980};
        case Armorslayer  : return {Sword,     D, 18, 11,  8,  80,  0, 1,  1260};
        case Longsword    : return {Sword,     D, 18, 11,  6,  85,  0, 1,  1260};
        case WoDao        : return {Sword,     D, 20,  5,  8,  75, 35, 1,  1200};
        case SteelBlade   : return {Sword,     C, 25, 14, 11,  65,  0, 2,  1250};
        case KillingEdge  : return {Sword,     C, 20,  7,  9,  75, 30, 1,  1300};
        case Wyrmslayer   : return {Sword,     C, 20,  5,  7,  75,  0, 1,  3000};
        case LightBlade   : return {Sword,     C, 25,  9,  9,  70,  0, 1,  1250};
        case Lancereaver  : return {Sword,     C, 15,  9,  9,  75,  5, 2,  1800};
        case BraveSword   : return {Sword,     B, 30, 12,  9,  75,  0, 1,  3000};
        case WindSword    : return {Sword,     B, 40,  9,  9,  70,  0, 1,  8000};
        case SilverSword  : return {Sword,     A, 20,  8, 13,  80,  0, 1,  1500};
        case SilverBlade  : return {Sword,     A, 15, 13, 14,  60,  0, 2,  1800};
        case RuneSword    : return {Sword,     A, 15, 11, 12,  65,  0, 1,  3300};
        case RegalBlade   : return {Sword,     S, 25,  9, 20,  85,  0, 1, 15000};
        case ManiKatti    : return {Sword,     U, 45,  3,  8,  80, 20, 2,    -1};
        case Rapier       : return {Sword,     U, 40,  5,  7,  95, 10, 2,  6000};
        case Durandal     : return {Sword,     U, 20, 16, 17,  90,  0, 1,    -1};
        case SolKatti     : return {Sword,     U, 30, 14, 12,  95, 25, 2,    -1};

        case IronLance    : return {Lance,     E, 45,  8,  7, 80,  0, 1,   360};
        case SlimLance    : return {Lance,     E, 30,  4,  4, 85,  5, 1,   450};
        case Javelin      : return {Lance,     E, 20, 11,  6, 65,  0, 1,   400};
        case EmblemLance  : return {Lance,     E, 60,  8,  7, 80,  0, 1,    -1};
        case PoisonLance  : return {Lance,     E, 40,  8,  4, 60,  0, 1,    -1};
        case SteelLance   : return {Lance,     D, 30, 13, 10, 70,  0, 2,   480};
        case HeavySpear   : return {Lance,     D, 16, 14,  9, 70,  0, 1,  1200};
        case Horseslayer  : return {Lance,     D, 16, 13,  7, 70,  0, 1,  1040};
        case ShortSpear   : return {Lance,     C, 18, 12,  9, 60,  0, 1,   900};
        case KillerLance  : return {Lance,     C, 20,  9, 10, 70, 30, 1,  1200};
        case Axereaver    : return {Lance,     C, 15, 11, 10, 70,  0, 2,  1950};
        case BraveLance   : return {Lance,     B, 30, 14, 10, 70,  0, 1,  7500};
        case Spear        : return {Lance,     B, 15, 10, 12, 70,  0, 1,  9000};
        case Spear2       : return {Lance,     B, 15, 10, 12, 70,  0, 1,  1500};
        case SilverLance  : return {Lance,     A, 20, 10, 14, 75,  0, 1,  1200};
        case RexHasta     : return {Lance,     S, 25, 11, 21, 80,  0, 1, 15000};

        case HandAxe      : return {Axe,       E, 20, 12,  7, 60,  0, 1,   300};
        case IronAxe      : return {Axe,       E, 45, 10,  8, 75,  0, 1,   270};
        case EmblemAxe    : return {Axe,       E, 60, 10,  8, 75,  0, 1,    -1};
        case SteelAxe     : return {Axe,       E, 30, 15, 11, 65,  0, 2,   360};
        case DevilAxe     : return {Axe,       E, 20, 18, 18, 55,  0, 8,   900};
        case PoisonAxe    : return {Axe,       D, 40, 10,  4, 60,  0, 1,    -1};
        case Halberd      : return {Axe,       D, 18, 15, 10, 60,  0, 1,   810};
        case Hammer       : return {Axe,       D, 20, 15, 10, 55,  0, 2,   800};
        case DragonAxe    : return {Axe,       C, 40, 11, 10, 60,  0, 1,  6000};
        case KillerAxe    : return {Axe,       C, 20, 11, 11, 65, 30, 1,  1000};
        case Swordreaver  : return {Axe,       C, 15, 13, 11, 65,  5, 2,  2100};
        case Swordslayer  : return {Axe,       C, 20, 13, 11, 80,  5, 1,  2000};
        case BraveAxe     : return {Axe,       B, 30, 16, 10, 65,  0, 1,  2250};
        case Tomahawk     : return {Axe,       A, 15, 14, 13, 65,  0, 1,  3000};
        case SilverAxe    : return {Axe,       A, 20, 12, 15, 70,  0, 1,  1000};
        case Basilikos    : return {Axe,       S, 25, 13, 22, 75,  0, 1, 15000};
        case WolfBeil     : return {Axe,       U, 30, 10, 10, 75,  5, 1,  6000};
        case Armads       : return {Axe,       U, 25, 18, 18, 85,  0, 1,    -1};

        case IronBow      : return {Bow,       E, 1, 1, 1, 1, 0, 1, 1};
        case EmblemBow    : return {Bow,       E, 1, 1, 1, 1, 0, 1, 1};
        case PoisonBow    : return {Bow,       E, 1, 1, 1, 1, 0, 1, 1};
        case ShortBow     : return {Bow,       E, 1, 1, 1, 1, 0, 1, 1};
        case Longbow      : return {Bow,       E, 1, 1, 1, 1, 0, 1, 1};
        case SteelBow     : return {Bow,       E, 1, 1, 1, 1, 0, 1, 1};
        case KillerBow    : return {Bow,       E, 1, 1, 1, 1, 0, 1, 1};
        case BraveBow     : return {Bow,       E, 1, 1, 1, 1, 0, 1, 1};
        case SilverBow    : return {Bow,       E, 1, 1, 1, 1, 0, 1, 1};
        case Rienfleche   : return {Bow,       E, 1, 1, 1, 1, 0, 1, 1};

        case Fire         : return {Anima,     E, 40,  4,  5, 90,  0, 1,   500};
        case Thunder      : return {Anima,     D, 35,  6,  8, 80,  5, 1,   700};
        case Elfire       : return {Anima,     C, 30, 10, 10, 85,  0, 2,  1200};
        case Bolting      : return {Anima,     B,  5, 20, 12, 60,  0, 1,  2500};
        case Fimbulvetr   : return {Anima,     A, 20, 12, 13, 80,  0, 1,  6000};
        case Excalibur    : return {Anima,     S, 25, 13, 18, 90, 10, 3, 25000};
        case Forblaze     : return {Anima,     U, 20, 11, 14, 85,  5, 1,    -1};

        case Lightning    : return {Light,     E, 1, 1, 1, 1, 0, 1, 1};
        case Shine        : return {Light,     E, 1, 1, 1, 1, 0, 1, 1};
        case Divine       : return {Light,     E, 1, 1, 1, 1, 0, 1, 1};
        case Purge        : return {Light,     E, 1, 1, 1, 1, 0, 1, 1};
        case Aura         : return {Light,     E, 1, 1, 1, 1, 0, 1, 1};
        case Aureola      : return {Light,     E, 1, 1, 1, 1, 0, 1, 1};
        case Luce         : return {Light,     E, 1, 1, 1, 1, 0, 1, 1};

        case Flux         : return {Dark,      E, 1, 1, 1, 1, 0, 1, 1};
        case Luna         : return {Dark,      E, 1, 1, 1, 1, 0, 1, 1};
        case Nosferatu    : return {Dark,      E, 1, 1, 1, 1, 0, 1, 1};
        case Eclipse      : return {Dark,      E, 1, 1, 1, 1, 0, 1, 1};
        case Fenrir       : return {Dark,      E, 1, 1, 1, 1, 0, 1, 1};
        case Gespenst     : return {Dark,      E, 1, 1, 1, 1, 0, 1, 1};
        case Ereshkigal   : return {Dark,      E, 1, 1, 1, 1, 0, 1, 1};

        case Heal         : return {Staff,     E, 1, 1, 1, 1, 0, 1, 1};
        case Mend         : return {Staff,     E, 1, 1, 1, 1, 0, 1, 1};
        case TorchStaff   : return {Staff,     E, 1, 1, 1, 1, 0, 1, 1};
        case Unlock       : return {Staff,     E, 1, 1, 1, 1, 0, 1, 1};
        case Recover      : return {Staff,     E, 1, 1, 1, 1, 0, 1, 1};
        case Restore      : return {Staff,     E, 1, 1, 1, 1, 0, 1, 1};
        case Hammerne     : return {Staff,     E, 1, 1, 1, 1, 0, 1, 1};
        case Barrier      : return {Staff,     E, 1, 1, 1, 1, 0, 1, 1};
        case Physic       : return {Staff,     E, 1, 1, 1, 1, 0, 1, 1};
        case Silence      : return {Staff,     E, 1, 1, 1, 1, 0, 1, 1};
        case Sleep        : return {Staff,     E, 1, 1, 1, 1, 0, 1, 1};
        case Berserk      : return {Staff,     E, 1, 1, 1, 1, 0, 1, 1};
        case Rescue       : return {Staff,     E, 1, 1, 1, 1, 0, 1, 1};
        case Fortify      : return {Staff,     E, 1, 1, 1, 1, 0, 1, 1};
        case Warp         : return {Staff,     E, 1, 1, 1, 1, 0, 1, 1};

        case DelphiShield : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case IronRune     : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case FilasMight   : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case NinisGrace   : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case ThorsIre     : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case SetsLitany   : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case AfasDrops    : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case DoorKey      : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case ChestKey     : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case ChestKey5    : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case Vulnerary    : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case Antitoxin    : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case Torch        : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case Mine         : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case LightRune    : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case PureWater    : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case Lockpick     : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case Elixir       : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case SilverCard   : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case MemberCard   : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case RedGem       : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case BlueGem      : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case WhiteGem     : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case AngelicRobe  : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case EnergyRing   : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case SecretBook   : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case Speedwing    : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case GoddessIcon  : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case Dracoshield  : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case Talisman     : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case Boots        : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case BodyRing     : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case HeroCrest    : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case KnightCrest  : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case OrionsBolt   : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case ElysianWhip  : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case GuidingRing  : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case EarthSeal    : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case HeavensSeal  : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case FellContract : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case OceanSeal    : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case EmblemSeal   : return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
        case Flametongue  : return {Dark,      E, 1, 1, 1, 1, 0, 1, 1};
        default:            return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
    }
}

std::string Item::getName()
{
    switch (id)
    {
        case None         : return "";
        case IronSword    : return "Iron Sword";
        case SlimSword    : return "Slim Sword";
        case EmblemBlade  : return "Emblem Blade";
        case PoisonSword  : return "Poison Sword";
        case SteelSword   : return "Steel Sword";
        case IronBlade    : return "Iron Blade";
        case Armorslayer  : return "Armorslayer";
        case Longsword    : return "Longsword";
        case WoDao        : return "Wo Dao";
        case SteelBlade   : return "Steel Blade";
        case KillingEdge  : return "Killing Edge";
        case Wyrmslayer   : return "Wyrmslayer";
        case LightBlade   : return "Light Blade";
        case Lancereaver  : return "Lancereaver";
        case BraveSword   : return "Brave Sword";
        case WindSword    : return "Wind Sword";
        case SilverSword  : return "Silver Sword";
        case SilverBlade  : return "Silver Blade";
        case RuneSword    : return "Rune Sword";
        case RegalBlade   : return "Regal Blade";
        case ManiKatti    : return "Mani Katti";
        case Rapier       : return "Rapier";
        case Durandal     : return "Durandal";
        case SolKatti     : return "Sol Katti";
        case IronLance    : return "Iron Lance";
        case SlimLance    : return "Slim Lance";
        case Javelin      : return "Javelin";
        case EmblemLance  : return "Emblem Lance";
        case PoisonLance  : return "Poison Lance";
        case SteelLance   : return "Steel Lance";
        case HeavySpear   : return "Heavy Spear";
        case Horseslayer  : return "Horseslayer";
        case ShortSpear   : return "Short Spear";
        case KillerLance  : return "Killer Lance";
        case Axereaver    : return "Axereaver";
        case BraveLance   : return "Brave Lance";
        case Spear        : return "Spear";
        case Spear2       : return "Spear2";
        case SilverLance  : return "Silver Lance";
        case RexHasta     : return "Rex Hasta";
        case HandAxe      : return "Hand Axe";
        case IronAxe      : return "Iron Axe";
        case EmblemAxe    : return "Emblem Axe";
        case SteelAxe     : return "Steel Axe";
        case DevilAxe     : return "Devil Axe";
        case PoisonAxe    : return "Poison Axe";
        case Halberd      : return "Halberd";
        case Hammer       : return "Hammer";
        case DragonAxe    : return "Dragon Axe";
        case KillerAxe    : return "Killer Axe";
        case Swordreaver  : return "Swordreaver";
        case Swordslayer  : return "Swordslayer";
        case BraveAxe     : return "Brave Axe";
        case Tomahawk     : return "Tomahawk";
        case SilverAxe    : return "Silver Axe";
        case Basilikos    : return "Basilikos";
        case WolfBeil     : return "Wolf Beil";
        case Armads       : return "Armads";
        case IronBow      : return "Iron Bow";
        case EmblemBow    : return "Emblem Bow";
        case PoisonBow    : return "Poison Bow";
        case ShortBow     : return "Short Bow";
        case Longbow      : return "Longbow";
        case SteelBow     : return "Steel Bow";
        case KillerBow    : return "Killer Bow";
        case BraveBow     : return "Brave Bow";
        case SilverBow    : return "Silver Bow";
        case Rienfleche   : return "Rienfleche";
        case Fire         : return "Fire";
        case Thunder      : return "Thunder";
        case Elfire       : return "Elfire";
        case Bolting      : return "Bolting";
        case Fimbulvetr   : return "Fimbulvetr";
        case Excalibur    : return "Excalibur";
        case Forblaze     : return "Forblaze";
        case Lightning    : return "Lightning";
        case Shine        : return "Shine";
        case Divine       : return "Divine";
        case Purge        : return "Purge";
        case Aura         : return "Aura";
        case Aureola      : return "Aureola";
        case Luce         : return "Luce";
        case Flux         : return "Flux";
        case Luna         : return "Luna";
        case Nosferatu    : return "Nosferatu";
        case Eclipse      : return "Eclipse";
        case Fenrir       : return "Fenrir";
        case Gespenst     : return "Gespenst";
        case Ereshkigal   : return "Ereshkigal";
        case Heal         : return "Heal";
        case Mend         : return "Mend";
        case TorchStaff   : return "Torch";
        case Unlock       : return "Unlock";
        case Recover      : return "Recover";
        case Restore      : return "Restore";
        case Hammerne     : return "Hammerne";
        case Barrier      : return "Barrier";
        case Physic       : return "Physic";
        case Silence      : return "Silence";
        case Sleep        : return "Sleep";
        case Berserk      : return "Berserk";
        case Rescue       : return "Rescue";
        case Fortify      : return "Fortify";
        case Warp         : return "Warp";
        case DelphiShield : return "Delphi Shield";
        case IronRune     : return "Iron Rune";
        case FilasMight   : return "Filas Might";
        case NinisGrace   : return "Ninis Grace";
        case ThorsIre     : return "Thors Ire";
        case SetsLitany   : return "Sets Litany";
        case AfasDrops    : return "Afas Drops";
        case DoorKey      : return "Door Key";
        case ChestKey     : return "Chest Key";
        case ChestKey5    : return "Chest Key";
        case Vulnerary    : return "Vulnerary";
        case Antitoxin    : return "Antitoxin";
        case Torch        : return "Torch";
        case Mine         : return "Mine";
        case LightRune    : return "Light Rune";
        case PureWater    : return "Pure Water";
        case Lockpick     : return "Lockpick";
        case Elixir       : return "Elixir";
        case SilverCard   : return "Silver Card";
        case MemberCard   : return "Member Card";
        case RedGem       : return "Red Gem";
        case BlueGem      : return "Blue Gem";
        case WhiteGem     : return "White Gem";
        case AngelicRobe  : return "Angelic Robe";
        case EnergyRing   : return "Energy Ring";
        case SecretBook   : return "Secret Book";
        case Speedwing    : return "Speedwing";
        case GoddessIcon  : return "Goddess Icon";
        case Dracoshield  : return "Dracoshield";
        case Talisman     : return "Talisman";
        case Boots        : return "Boots";
        case BodyRing     : return "Body Ring";
        case HeroCrest    : return "Hero Crest";
        case KnightCrest  : return "Knight Crest";
        case OrionsBolt   : return "Orions Bolt";
        case ElysianWhip  : return "Elysian Whip";
        case GuidingRing  : return "Guiding Ring";
        case EarthSeal    : return "Earth Seal";
        case HeavensSeal  : return "Heavens Seal";
        case FellContract : return "Fell Contract";
        case OceanSeal    : return "Ocean Seal";
        case EmblemSeal   : return "Emblem Seal";
        case Flametongue  : return "Flametongue";
        default:            return "";
    }
}
