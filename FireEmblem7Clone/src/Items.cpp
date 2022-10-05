#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <unordered_set>

#include "Weapons.hpp"
#include "Unit.hpp"
#include "Global.hpp"
#include "Items.hpp"


SDL_Texture* Item::itemsTex = nullptr;

void Item::init()
{
    itemsTex = IMG_LoadTexture(Global::sdlRenderer, "res/Images/Items/Items.png");
    if (itemsTex == nullptr)
    {
        printf("Error: could not load 'res/Images/Items/Items.png'\n");   
    }
}

Item::Item()
{
    id = None;
    usesRemaining = 0;
}

bool Item::operator ==(const Item& other) const
{
    return id == other.id && usesRemaining == other.usesRemaining;
}

Item::Item(ItemId id)
{
    this->id = id;
    WeaponStats stats = getWeaponStats();
    usesRemaining = stats.usesMax;
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

bool Item::isStaff()
{
    int idx = (int)id;
    if (idx >= (int)Heal && idx < (int)DelphiShield)
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
            unit->unitResources.growthHp  += 5;
            unit->unitResources.growthStr += 5;
            unit->unitResources.growthMag += 5;
            unit->unitResources.growthSkl += 5;
            unit->unitResources.growthSpd += 5;
            unit->unitResources.growthLck += 5;
            unit->unitResources.growthDef += 5;
            unit->unitResources.growthRes += 5;
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

        case AngelicRobe:
        {
            unit->maxHp += 7;
            unit->hp += 7;
            return;
        }

        case EnergyRing:
        {
            unit->str += 2;
            return;
        }

        case SecretBook:
        {
            unit->skl += 2;
            return;
        }

        case Speedwing:
        {
            unit->spd += 2;
            return;
        }

        case GoddessIcon:
        {
            unit->lck += 2;
            return;
        }

        case Dracoshield:
        {
            unit->def += 2;
            return;
        }

        case Talisman:
        {
            unit->res += 2;
            return;
        }

        case Boots:
        {
            unit->mov += 2;
            return;
        }

        case BodyRing:
        {
            unit->con += 2;
            return;
        }
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

std::unordered_set<int> Item::getStaffRange()
{
    std::unordered_set<int> ranges;

    if (!isStaff())
    {
        return ranges;
    }

    if (id == Torch   ||
        id == Physic  ||
        id == Silence ||
        id == Sleep   ||
        id == Berserk ||
        id == Rescue)
    {
        ranges.insert(1);
        ranges.insert(2);
        ranges.insert(3);
        ranges.insert(4);
        ranges.insert(5);
        ranges.insert(6);
        ranges.insert(7);
        ranges.insert(8);
        ranges.insert(9);
        ranges.insert(10);
    }
    else if (id == Unlock)
    {
        ranges.insert(1);
        ranges.insert(2);
    }
    else
    {
        ranges.insert(1);
    }

    return ranges;
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

        case IronBow      : return {Bow,       E, 45,  5,  6, 85,  0, 1,   540};
        case EmblemBow    : return {Bow,       E, 60,  5,  6, 85,  0, 1,    -1};
        case PoisonBow    : return {Bow,       D, 40,  5,  4, 65,  0, 1,    -1};
        case ShortBow     : return {Bow,       D, 22,  3,  5, 85, 10, 2,  1760};
        case Longbow      : return {Bow,       D, 20, 10,  5, 65,  0, 1,  2000};
        case SteelBow     : return {Bow,       D, 30,  9,  9, 70,  0, 1,   720};
        case KillerBow    : return {Bow,       C, 20,  7,  9, 75, 30, 1,  1400};
        case BraveBow     : return {Bow,       B, 30, 12, 10, 70,  0, 1,  7500};
        case SilverBow    : return {Bow,       A, 20,  6, 13, 75,  0, 1,  1600};
        case Rienfleche   : return {Bow,       S, 25,  7, 20, 80,  0, 1, 15000};

        case Fire         : return {Anima,     E, 40,  4,  5, 90,  0, 1,   500};
        case Thunder      : return {Anima,     D, 35,  6,  8, 80,  5, 1,   700};
        case Elfire       : return {Anima,     C, 30, 10, 10, 85,  0, 2,  1200};
        case Bolting      : return {Anima,     B,  5, 20, 12, 60,  0, 1,  2500};
        case Fimbulvetr   : return {Anima,     A, 20, 12, 13, 80,  0, 1,  6000};
        case Excalibur    : return {Anima,     S, 25, 13, 18, 90, 10, 3, 25000};
        case Forblaze     : return {Anima,     U, 20, 11, 14, 85,  5, 1,    -1};

        case Lightning    : return {Light,     E, 35,  6,  4, 95,  5, 1,   630};
        case Shine        : return {Light,     D, 30,  8,  6, 90,  8, 2,   900};
        case Divine       : return {Light,     C, 25, 12,  8, 85, 10, 1,  2500};
        case Purge        : return {Light,     B,  5, 20, 10, 75,  5, 3,  3000};
        case Aura         : return {Light,     A, 20, 15, 12, 85, 15, 1,  8000};
        case Aureola      : return {Light,     S, 20, 14, 15, 90,  5, 0,    -1};
        case Luce         : return {Light,     S, 25, 16, 16, 95, 25, 3, 30000};

        case Flux         : return {Dark,      D, 45,  8,  7, 80,  0, 1,   900};
        case Luna         : return {Dark,      C, 35, 12,  0, 95, 20, 1,  5250};
        case Nosferatu    : return {Dark,      C, 20, 14, 10, 70,  0, 1,  3200};
        case Eclipse      : return {Dark,      B,  5, 12,  0, 30,  0, 8,  4000};
        case Fenrir       : return {Dark,      A, 20, 18, 15, 70,  0, 1,  9000};
        case Gespenst     : return {Dark,      S, 25, 20, 23, 80,  0, 3, 35000};
        case Ereshkigal   : return {Dark,      S, 50, 12, 20, 95,  0, 1,    -1};

        case Heal         : return {Staff,     E, 30, 0, 11, 0, 0, 2,  600};
        case Mend         : return {Staff,     D, 20, 0, 12, 0, 0, 3, 1000};
        case TorchStaff   : return {Staff,     D, 10, 0, 15, 0, 0, 5, 1000};
        case Unlock       : return {Staff,     D, 10, 0, 17, 0, 0, 5, 1500};
        case Recover      : return {Staff,     C, 15, 0, 17, 0, 0, 3, 2250};
        case Restore      : return {Staff,     C, 10, 0, 20, 0, 0, 3, 3000};
        case Hammerne     : return {Staff,     C,  3, 0, 40, 0, 0, 8, 2000};
        case Barrier      : return {Staff,     C, 15, 0, 17, 0, 0, 4, 2250};
        case Physic       : return {Staff,     B, 15, 0, 22, 0, 0, 3, 3750};
        case Silence      : return {Staff,     B,  3, 0, 30, 0, 0, 5, 1200};
        case Sleep        : return {Staff,     B,  3, 0, 35, 0, 0, 5, 1500};
        case Berserk      : return {Staff,     B,  3, 0, 40, 0, 0, 5, 1800};
        case Rescue       : return {Staff,     B,  3, 0, 40, 0, 0, 7, 1800};
        case Fortify      : return {Staff,     A,  8, 0, 60, 0, 0, 5, 8000};
        case Warp         : return {Staff,     A,  5, 0, 85, 0, 0, 7, 7500};

        case Flametongue  : return {Dark,      E, 1, 1, 1, 1, 0, 1, 1};
        default:            return {NotWeapon, N, 1, 1, 1, 1, 0, 1, 1};
    }
}

StaffStats Item::getStaffStats()
{
    switch (id)
    {
        case Heal         : return {E, 30, 11, 2,  600};
        case Mend         : return {D, 20, 12, 3, 1000};
        case TorchStaff   : return {D, 10, 15, 5, 1000};
        case Unlock       : return {D, 10, 17, 5, 1500};
        case Recover      : return {C, 15, 17, 3, 2250};
        case Restore      : return {C, 10, 20, 3, 3000};
        case Hammerne     : return {C,  3, 40, 8, 2000};
        case Barrier      : return {C, 15, 17, 4, 2250};
        case Physic       : return {B, 15, 22, 3, 3750};
        case Silence      : return {B,  3, 30, 5, 1200};
        case Sleep        : return {B,  3, 35, 5, 1500};
        case Berserk      : return {B,  3, 40, 5, 1800};
        case Rescue       : return {B,  3, 40, 7, 1800};
        case Fortify      : return {A,  8, 60, 5, 8000};
        case Warp         : return {A,  5, 85, 7, 7500};
        default           : return {N,  0,  0, 0,    0};
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
