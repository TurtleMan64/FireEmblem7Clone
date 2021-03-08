#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Weapons.hpp"
#include "Items.hpp"
#include "ClassData.hpp"

class Sprite;

class Unit
{
private:
    class UnitResources
    {
    public:
        std::string name;
        std::string mugshotPath;
        std::string mugshotTinyPath;
        std::string mapPath;
        std::string battlePath;
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

    std::unordered_map<std::string, UnitResources> unitResources;

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
    std::string displayName;
    ClassType classType;

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

    int growthHp  = 0;
    int growthStr = 0;
    int growthMag = 0;
    int growthSkl = 0;
    int growthSpd = 0;
    int growthLck = 0;
    int growthDef = 0;
    int growthRes = 0;

    WeaponRank weaponRank[9] = {N,N,N,N,N,N,N,N,N};

    std::vector<Item> items;

    Unit* rescuedUnit = nullptr;

    Unit(std::string displayName, std::string className);
    ~Unit();

    void render(int x, int y, int spriteIndex, int viewportPixelOffsetX, int viewportPixelOffsetY);

    std::unordered_set<int> getAttackRanges();

    std::unordered_set<int> getEquipWeaponAttackRange();

    Item* getEquippedWeapon();

    int getAttackSpeedWithWeapon(Item weapon);

    void calculateCombatStatsVsUnit(Unit* other, int* damage, int* hit, int* crit);
};
