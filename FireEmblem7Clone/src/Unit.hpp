#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Weapons.hpp"
#include "Items.hpp"
#include "ClassData.hpp"

class Sprite;

class UnitResources
{
public:
    std::string displayName;
    std::string mugshotPath;
    std::string mugshotTinyPath;

    int growthHp  = 0;
    int growthStr = 0;
    int growthMag = 0;
    int growthSkl = 0;
    int growthSpd = 0;
    int growthLck = 0;
    int growthDef = 0;
    int growthRes = 0;

    UnitResources();
    UnitResources(std::string filePath);
};

class Unit
{
private:
    std::unordered_map<std::string, UnitResources> unitNameToResources;

    Sprite* sprMapIdleB     = nullptr;
    Sprite* sprMapHuzzahB   = nullptr;
    Sprite* sprMapRunUpB    = nullptr;
    Sprite* sprMapRunDownB  = nullptr;
    Sprite* sprMapRunLeftB  = nullptr;

    Sprite* sprMapIdleR     = nullptr;
    Sprite* sprMapHuzzahR   = nullptr;
    Sprite* sprMapRunUpR    = nullptr;
    Sprite* sprMapRunDownR  = nullptr;
    Sprite* sprMapRunLeftR  = nullptr;

public:
    UnitResources unitResources;
    ClassResources classResources;

    int spriteIndex = 0;
    Sprite* sprMugshot      = nullptr;
    Sprite* sprMugshotTiny  = nullptr;

    //pixel coordinates
    int x = 0;
    int y = 0;

    //Map specific data
    int tileX = 0; //position on the map
    int tileY = 0;
    bool isUsed = false;

    //Character specific data
    int lvl = 1;
    int exp = 0;

    int maxHp = 1;
    int hp = 1;

    int str = 0;
    int mag = 0;
    int skl = 0;
    int spd = 0;
    int lck = 0;
    int def = 0;
    int res = 0;

    int mov = 0;
    int con = 0;
    int aid = 0;

    int battleTotal  = 0;
    int battleWins   = 0;
    int battleLosses = 0;

    int weaponRank[9] = {N,N,N,N,N,N,N,N,N};

    std::vector<Item> items;

    Unit* rescuedUnit = nullptr;

    Unit(std::string unitName, std::string className);
    ~Unit();

    void render(int x, int y, int spriteIndex, int viewportPixelOffsetX, int viewportPixelOffsetY);
    void render(int x, int y, int spriteIndex, int viewportPixelOffsetX, int viewportPixelOffsetY, SDL_Color color);

    std::unordered_set<int> getAttackRanges();

    std::unordered_set<int> getEquipWeaponAttackRange();

    Item* getEquippedWeapon();

    int getAttackSpeedWithWeapon(WeaponStats weaponStats);

    bool canUseWeapon(Item weapon);

    void calculateCombatStatsVsUnit(Unit* other, int* damage, int* hit, int* crit);

    void calculateBaseCombatStats(int* attack, int* hit, int* avoid, int* crit, int* attackSpeed);
};
