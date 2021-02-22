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

    switch (getWeaponType())
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

WeaponType Item::getWeaponType()
{
    int idx = (int)id;

    if (idx == 0)
    {
        return NotWeapon;
    }
    else if (idx < (int)IronLance)
    {
        return Sword;
    }
    else if (idx < (int)HandAxe)
    {
        return Lance;
    }
    else if (idx < (int)IronBow)
    {
        return Axe;
    }
    else if (idx < (int)Fire)
    {
        return Bow;
    }
    else if (idx < (int)Lightning)
    {
        return Anima;
    }
    else if (idx < (int)Flux)
    {
        return Light;
    }
    else if (idx < (int)Heal)
    {
        return Dark;
    }
    else
    {
        return NotWeapon;
    }
}

WeaponRank Item::getWeaponRankRequirement()
{
    switch (id)
    {
        case None         : return N;
        case IronSword    : return E;
        case SlimSword    : return E;
        case EmblemBlade  : return E;
        case PoisonSword  : return E;
        case SteelSword   : return E;
        case IronBlade    : return E;
        case Armorslayer  : return E;
        case Longsword    : return E;
        case WoDao        : return E;
        case SteelBlade   : return E;
        case KillingEdge  : return E;
        case Wyrmslayer   : return E;
        case LightBlade   : return E;
        case Lancereaver  : return E;
        case BraveSword   : return E;
        case WindSword    : return E;
        case SilverSword  : return E;
        case SilverBlade  : return E;
        case RuneSword    : return E;
        case RegalBlade   : return E;
        case ManiKatti    : return E;
        case Rapier       : return E;
        case Durandal     : return E;
        case SolKatti     : return E;
        case IronLance    : return E;
        case SlimLance    : return E;
        case Javelin      : return E;
        case EmblemLance  : return E;
        case PoisonLance  : return E;
        case SteelLance   : return E;
        case HeavySpear   : return E;
        case Horseslayer  : return E;
        case ShortSpear   : return E;
        case KillerLance  : return E;
        case Axereaver    : return E;
        case BraveLance   : return E;
        case Spear        : return E;
        case Spear2       : return E;
        case SilverLance  : return E;
        case RexHasta     : return E;
        case HandAxe      : return E;
        case IronAxe      : return E;
        case EmblemAxe    : return E;
        case SteelAxe     : return E;
        case DevilAxe     : return E;
        case PoisonAxe    : return E;
        case Halberd      : return E;
        case Hammer       : return E;
        case DragonAxe    : return E;
        case KillerAxe    : return E;
        case Swordreaver  : return E;
        case Swordslayer  : return E;
        case BraveAxe     : return E;
        case Tomahawk     : return E;
        case SilverAxe    : return E;
        case Basilikos    : return E;
        case WolfBeil     : return E;
        case Armads       : return E;
        case IronBow      : return E;
        case EmblemBow    : return E;
        case PoisonBow    : return E;
        case ShortBow     : return E;
        case Longbow      : return E;
        case SteelBow     : return E;
        case KillerBow    : return E;
        case BraveBow     : return E;
        case SilverBow    : return E;
        case Rienfleche   : return E;
        case Fire         : return E;
        case Thunder      : return E;
        case Elfire       : return E;
        case Bolting      : return E;
        case Fimbulvetr   : return E;
        case Excalibur    : return E;
        case Forblaze     : return E;
        case Lightning    : return E;
        case Shine        : return E;
        case Divine       : return E;
        case Purge        : return E;
        case Aura         : return E;
        case Aureola      : return E;
        case Luce         : return E;
        case Flux         : return E;
        case Luna         : return E;
        case Nosferatu    : return E;
        case Eclipse      : return E;
        case Fenrir       : return E;
        case Gespenst     : return E;
        case Ereshkigal   : return E;
        case Heal         : return E;
        case Mend         : return E;
        case TorchStaff   : return E;
        case Unlock       : return E;
        case Recover      : return E;
        case Restore      : return E;
        case Hammerne     : return E;
        case Barrier      : return E;
        case Physic       : return E;
        case Silence      : return E;
        case Sleep        : return E;
        case Berserk      : return E;
        case Rescue       : return E;
        case Fortify      : return E;
        case Warp         : return E;
        case DelphiShield : return N;
        case IronRune     : return N;
        case FilasMight   : return N;
        case NinisGrace   : return N;
        case ThorsIre     : return N;
        case SetsLitany   : return N;
        case AfasDrops    : return N;
        case DoorKey      : return N;
        case ChestKey     : return N;
        case ChestKey5    : return N;
        case Vulnerary    : return N;
        case Antitoxin    : return N;
        case Torch        : return N;
        case Mine         : return N;
        case LightRune    : return N;
        case PureWater    : return N;
        case Lockpick     : return N;
        case Elixir       : return N;
        case SilverCard   : return N;
        case MemberCard   : return N;
        case RedGem       : return N;
        case BlueGem      : return N;
        case WhiteGem     : return N;
        case AngelicRobe  : return N;
        case EnergyRing   : return N;
        case SecretBook   : return N;
        case Speedwing    : return N;
        case GoddessIcon  : return N;
        case Dracoshield  : return N;
        case Talisman     : return N;
        case Boots        : return N;
        case BodyRing     : return N;
        case HeroCrest    : return N;
        case KnightCrest  : return N;
        case OrionsBolt   : return N;
        case ElysianWhip  : return N;
        case GuidingRing  : return N;
        case EarthSeal    : return N;
        case HeavensSeal  : return N;
        case FellContract : return N;
        case OceanSeal    : return N;
        case EmblemSeal   : return N;
        case Flametongue  : return E;
        default:            return N;
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
