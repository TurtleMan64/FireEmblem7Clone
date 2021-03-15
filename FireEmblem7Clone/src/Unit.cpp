#include <SDL/SDL.h>

#include <string>
#include <vector>
#include <unordered_set>

#include "Weapons.hpp"
#include "Items.hpp"
#include "Global.hpp"
#include "Sprite.hpp"
#include "Util.hpp"
#include "Map.hpp"
#include "Unit.hpp"

Unit::UnitResources::UnitResources() {}
Unit::UnitResources::UnitResources(std::string filePath)
{
    std::vector<std::string> file = Util::readFile(filePath);
    name            = file[0];
    mugshotPath     = file[1];
    mugshotTinyPath = file[2];
    mapPath         = file[3];
    battlePath      = file[4];

    std::vector<std::string> statGrowths = Util::split(file[5], ' ');
    growthHp  = stoi(statGrowths[0]);
    growthStr = stoi(statGrowths[1]);
    growthMag = stoi(statGrowths[2]);
    growthSkl = stoi(statGrowths[3]);
    growthSpd = stoi(statGrowths[4]);
    growthLck = stoi(statGrowths[5]);
    growthDef = stoi(statGrowths[6]);
    growthRes = stoi(statGrowths[7]);
}

Unit::Unit(std::string displayName, std::string className)
{
    this->displayName = displayName;

    if (unitResources.find(displayName) == unitResources.end() &&
        unitResources.find(className)   == unitResources.end())
    {
        std::string displayNameFile = "res/Characters/" + displayName + ".unit";
        std::string classNameFile   = "res/Characters/" + className   + ".unit";
        if (Util::fileExists(displayNameFile))
        {
            UnitResources resources = UnitResources(displayNameFile);
            unitResources[displayName] = resources;
        }
        else if (Util::fileExists(classNameFile))
        {
            UnitResources resources = UnitResources(classNameFile);
            unitResources[className] = resources;
        }
        else
        {
            printf("Error: Cannot find unit data for unit %s or class %s\n", displayName.c_str(), className.c_str());
        }
    }

    UnitResources resources;
    if (unitResources.find(displayName) != unitResources.end())
    {
        resources = unitResources[displayName];
    }
    else
    {
        resources = unitResources[className];
    }
    sprMapIdleB     = new Sprite(resources.mapPath + "/Idle",    0, 0, false);
    sprMapHuzzahB   = new Sprite(resources.mapPath + "/Huzzah",  0, 0, false);
    sprMapRunUpB    = new Sprite(resources.mapPath + "/RunUp",   0, 0, false);
    sprMapRunDownB  = new Sprite(resources.mapPath + "/RunDown", 0, 0, false);
    sprMapRunLeftB  = new Sprite(resources.mapPath + "/RunLeft", 0, 0, false);
    sprMapIdleR     = new Sprite(resources.mapPath + "/Idle",    0, 0, true);
    sprMapHuzzahR   = new Sprite(resources.mapPath + "/Huzzah",  0, 0, true);
    sprMapRunUpR    = new Sprite(resources.mapPath + "/RunUp",   0, 0, true);
    sprMapRunDownR  = new Sprite(resources.mapPath + "/RunDown", 0, 0, true);
    sprMapRunLeftR  = new Sprite(resources.mapPath + "/RunLeft", 0, 0, true);
    sprMugshot      = new Sprite(resources.mugshotPath,          0, 0, false);
    sprMugshotTiny  = new Sprite(resources.mugshotTinyPath,      0, 0, false);

    mov = 9;
}

Unit::~Unit()
{
    delete sprMapIdleR;
    delete sprMapHuzzahR;
    delete sprMapRunUpR;
    delete sprMapRunDownR;
    delete sprMapRunLeftR;
    delete sprMapIdleB;
    delete sprMapHuzzahB;
    delete sprMapRunUpB;
    delete sprMapRunDownB;
    delete sprMapRunLeftB;
    delete sprMugshot;
    delete sprMugshotTiny;
}

void Unit::render(int pixelX, int pixelY, int sprIndex, int viewportPixelOffsetX, int viewportPixelOffsetY)
{
    x = pixelX;
    y = pixelY;
    spriteIndex = sprIndex;

    Sprite* s = nullptr;
    switch (sprIndex)
    {
        case  0: s = sprMapIdleB;    break;
        case  1: s = sprMapHuzzahB;  break;
        case  2: s = sprMapRunUpB;   break;
        case  3: s = sprMapRunDownB; break;
        case  4: s = sprMapRunLeftB; sprMapRunLeftB->scaleX =  1; break;
        case  5: s = sprMapRunLeftB; sprMapRunLeftB->scaleX = -1; break;
        case  6: s = sprMapIdleR;    break;
        case  7: s = sprMapHuzzahR;  break;
        case  8: s = sprMapRunUpR;   break;
        case  9: s = sprMapRunDownR; break;
        case 10: s = sprMapRunLeftR; sprMapRunLeftR->scaleX =  1; break;
        case 11: s = sprMapRunLeftR; sprMapRunLeftR->scaleX = -1; break;
        default: s = sprMapIdleB; break;
    }
    s->x = (int)(viewportPixelOffsetX + pixelX);
    s->y = (int)(viewportPixelOffsetY + pixelY);
    s->imageIndex = Global::frameCount;
    SDL_Color c;
    if (!isUsed)
    {
        c = {255, 255, 255, 255};
    }
    else
    {
        c = {128, 128, 128, 255};
    }
    s->render(c);
}

std::unordered_set<int> Unit::getAttackRanges()
{
    std::unordered_set<int> ranges;
    for (int i = 0; i < items.size(); i++)
    {
        Item item = items[i];

        if (item.isWeapon())
        {
            WeaponStats stats = item.getWeaponStats();

            if (weaponRank[stats.type] >= stats.rankRequirement)
            {
                std::unordered_set<int> range = item.getWeaponRange();

                ranges.insert(range.begin(), range.end());
            }
        }
    }

    return ranges;
}

std::unordered_set<int> Unit::getEquipWeaponAttackRange()
{
    for (int i = 0; i < items.size(); i++)
    {
        Item item = items[i];

        if (item.isWeapon())
        {
            WeaponStats stats = item.getWeaponStats();

            if (weaponRank[stats.type] >= stats.rankRequirement)
            {
                return item.getWeaponRange();
            }
        }
    }

    std::unordered_set<int> empty;
    return empty;
}

Item* Unit::getEquippedWeapon()
{
    for (int i = 0; i < items.size(); i++)
    {
        Item item = items[i];

        if (item.isWeapon())
        {
            WeaponStats stats = item.getWeaponStats();

            if (weaponRank[stats.type] >= stats.rankRequirement)
            {
                return &items[i];
            }
        }
    }

    return nullptr;
}

int Unit::getAttackSpeedWithWeapon(WeaponStats weaponStats)
{
    int weightPenalty = weaponStats.weight - con;
    if (weightPenalty < 0)
    {
        weightPenalty = 0;
    }

    return spd - weightPenalty;
}

bool Unit::canUseWeapon(Item weapon)
{
    if (weapon.isWeapon())
    {
        WeaponStats stats = weapon.getWeaponStats();

        if (weaponRank[stats.type] >= stats.rankRequirement)
        {
            return true;
        }
    }

    return false;
}

void Unit::calculateCombatStatsVsUnit(Unit* other, int* damage, int* hit, int* crit)
{
    Item* myWeapon = getEquippedWeapon();
    if (myWeapon == nullptr)
    {
        *damage = 0;
        *hit = 0;
        *crit = 0;
        return;
    }

    int distanceToOther = Util::getManhattanDistance(this, other);

    WeaponStats myWeaponStats = myWeapon->getWeaponStats();
    std::unordered_set<int> range = myWeapon->getWeaponRange();

    if (range.find(distanceToOther) == range.end())
    {
        printf("Weapon not in range\n");
        *damage = 0;
        *hit = 0;
        *crit = 0;
        return;
    }

    Item* otherWeapon = other->getEquippedWeapon();
    WeaponStats otherWeaponStats;
    if (otherWeapon != nullptr)
    {
        otherWeaponStats = otherWeapon->getWeaponStats();
    }

    int weaponTriangle = Util::getWeaponTriangle(myWeaponStats, otherWeaponStats);

    MapTile myTile = Map::tiles[tileX + tileY*Map::tilesWidth];
    bool usesTile = true;
    if (classType == ClassType::Pegasus ||
        classType == ClassType::Wyvern)
    {
        usesTile = false;
    }

    MapTile otherTile = Map::tiles[other->tileX + other->tileY*Map::tilesWidth];
    bool otherUsesTile = true;
    if (other->classType == ClassType::Pegasus ||
        other->classType == ClassType::Wyvern)
    {
        otherUsesTile = false;
    }

    //Attack
    int supportBonusAttack = 0;
    bool isMagicWeapon = 
        (myWeaponStats.type == Anima ||
         myWeaponStats.type == Light ||
         myWeaponStats.type == Dark);

    int attack = myWeaponStats.might + weaponTriangle + supportBonusAttack;
    int otherDef = 0;
    if (!isMagicWeapon)
    {
        attack += str;
        otherDef += other->def;

        if (otherUsesTile)
        {
            otherDef += otherTile.defense;
        }
    }
    else
    {
        attack += mag;
        otherDef += other->res;
    }

    int finalDamage = attack - otherDef;
    if (finalDamage < 0)
    {
        finalDamage = 0;
    }
    *damage = finalDamage;


    //Hit
    int supportBonusHit = 0;
    int sRankBonusHit = 0;
    if (weaponRank[myWeaponStats.type] >= S)
    {
        sRankBonusHit = 5;
    }
    int tacticanBonusHit = 0;
    int myHit = myWeaponStats.hit + skl*2 + lck/2 + weaponTriangle*15 + supportBonusHit + sRankBonusHit + tacticanBonusHit;

    //Other avoid
    int otherSupportBonusAvoid = 0;
    int otherTerrainBonusAvoid = otherTile.avoid;
    if (!usesTile)
    {
        otherTerrainBonusAvoid = 0;
    }
    int otherTacticanBonusAvoid = 0;
    int otherAvoid = other->getAttackSpeedWithWeapon(otherWeaponStats)*2 + other->lck + otherSupportBonusAvoid + otherTerrainBonusAvoid + otherTacticanBonusAvoid;

    int finalHit = Util::clamp(0, myHit - otherAvoid, 100);
    *hit = finalHit;


    //Crit
    int supportBonusCrit = 0;
    int criticalBonus = 0;
    int sRankBonusCrit = 0;
    if (weaponRank[myWeaponStats.type] >= S)
    {
        sRankBonusCrit = 5;
    }
    int myCrit = myWeaponStats.crit + skl/2 + supportBonusCrit + criticalBonus + sRankBonusCrit;

    //Other Crit evade
    int otherSupportBonusEvade = 0;
    int otherTacticanBonusEvade = 0;
    int otherCritEvade = other->lck + otherSupportBonusEvade + otherTacticanBonusEvade;

    int finalCrit = Util::clamp(0, myCrit - otherCritEvade, 100);
    *crit = finalCrit;
}
