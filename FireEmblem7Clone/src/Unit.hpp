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

enum Behavior
{
    Roam = 0,
    WaitUntilEnemyInRange = 1,
    NoMove = 2,
    HeadForSieze = 3,
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

    Sprite* sprBossIcon = nullptr;

public:
    UnitResources unitResources;
    ClassResources classResources;

    int spriteIndex = 0;
    Sprite* sprMugshot      = nullptr;
    Sprite* sprMugshotTiny  = nullptr;

    //pixel coordinates. normally the same as tileX*16, tileY*16, except in the case of animations playing (like walking, attacking)
    int x = 0;
    int y = 0;

    //Map specific data
    int tileX = 0; //position on the map
    int tileY = 0;
    bool isUsed = false;

    //Enemy related stuff
    bool isBoss = false;
    Behavior behavior = NoMove;
    int groupId = 0;

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
    void renderHealthbar(int x, int y, int spriteIndex, int viewportPixelOffsetX, int viewportPixelOffsetY);

    std::unordered_set<int> getAttackRanges();

    std::unordered_set<int> getEquipWeaponAttackRange();

    Item* getEquippedWeapon();

    // Equips the weapon by moving it to the first slot in the inventory.
    // The given weapon should exist in the intentory already.
    void equipWeapon(Item weapon);

    // Moves the weapon at the given index to the beginning of the item list.
    void equipWeapon(int index);

    int getAttackSpeedWithWeapon(WeaponStats weaponStats);

    bool canUseWeapon(Item weapon);

    bool canUseStaff(Item staff);

    //void calculateCombatStatsVsUnit(Unit* other, int* damage, int* hit, int* crit);

    // Calculates the damage, hit, and crit for 2 units in combat.
    static void calculateCombatStatsVsUnit(
        Unit* me,    int myTileX,    int myTileY,    Item* myWeapon,
        Unit* other, int otherTileX, int otherTileY, Item* otherWeapon,
        int* myDamage,    int* myHit,    int* myCrit,
        int* otherDamage, int* otherHit, int* otherCrit);

    // Calculates the damage, hit, and crit when battling the other unit.
    void calculateCombatStatsVsUnit(Unit* other, int* damage, int* hit, int* crit, int* otherDamage, int* otherHit, int* otherCrit);

private:
    static void calculateCombatStatsVsUnit(
        Unit* me,    int myTileX,    int myTileY,    Item* myWeapon,
        Unit* other, int otherTileX, int otherTileY, Item* otherWeapon,
        int* damage, int* hit, int* crit);

public:
    // Calculates the damage, hit, and crit for 2 units in combat.
    //void calculateCombatStatsVsUnit(int myTileX, int myTileY, Item* myWeapon, Unit* other, int* damage, int* hit, int* crit);

    void calculateBaseCombatStats(int* attack, int* hit, int* avoid, int* crit, int* attackSpeed);
};
