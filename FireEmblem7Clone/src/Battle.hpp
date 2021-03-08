#pragma once

#include <vector>

#include "Unit.hpp"
#include "Weapons.hpp"
#include "Items.hpp"

struct TurnResult
{
    Unit* unitLeft  = nullptr;
    Unit* unitRight = nullptr;

    bool missed = false;
    bool crit   = false;

    int unitLeftHp  = 0;
    int unitLeftAtk = 0;
    int unitLeftHit = 0;
    int unitLeftCrt = 0;

    int unitRightHp  = 0;
    int unitRightAtk = 0;
    int unitRightHit = 0;
    int unitRightCrt = 0;

    Item leftWeapon  = None;
    Item rightWeapon = None;
};

class Battle
{
public:
    static Unit* unitLeft;
    static Unit* unitRight;

    static int turnIdx;
    static std::vector<int> turns;

    static std::vector<TurnResult> results;

    static void doBattle(Unit* attackingUnit, Unit* defendingUnit);
};
