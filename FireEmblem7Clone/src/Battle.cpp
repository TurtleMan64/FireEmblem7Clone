#include <vector>

#include "Unit.hpp"
#include "Util.hpp"
#include "Weapons.hpp"
#include "Items.hpp"
#include "Battle.hpp"

Unit* Battle::unitLeft;
Unit* Battle::unitRight;

int Battle::turnIdx;
std::vector<int> Battle::turns;

std::vector<TurnResult> Battle::results;

void Battle::doBattle(Unit* attackingUnit, Unit* defendingUnit)
{
    turnIdx = 0;
    turns.clear();
    results.clear();

    int leftAttackSpeed = attackingUnit->spd;
    Item* leftWeapon = attackingUnit->getEquippedWeapon();
    if (leftWeapon != nullptr)
    {
        leftAttackSpeed = attackingUnit->getAttackSpeedWithWeapon(leftWeapon->getWeaponStats());
    }
            
    int rightAttackSpeed = defendingUnit->spd;
    Item* rightWeapon = defendingUnit->getEquippedWeapon();
    if (rightWeapon != nullptr)
    {
        rightAttackSpeed = defendingUnit->getAttackSpeedWithWeapon(rightWeapon->getWeaponStats());
    }

    int leftAttack  = 0;
    int leftHit     = 0;
    int leftCrit    = 0;
    int rightAttack = 0;
    int rightHit    = 0;
    int rightCrit   = 0;
    attackingUnit->calculateCombatStatsVsUnit(defendingUnit, &leftAttack,  &leftHit,  &leftCrit);
    defendingUnit->calculateCombatStatsVsUnit(attackingUnit, &rightAttack, &rightHit, &rightCrit);

    if (leftWeapon != nullptr)
    {
        turns.push_back(0);
    }

    if (rightWeapon != nullptr)
    {
        turns.push_back(1);
    }

    if (leftWeapon != nullptr && leftAttackSpeed - rightAttackSpeed >= 4)
    {
        turns.push_back(0);
    }

    if (rightWeapon != nullptr && rightAttackSpeed - leftAttackSpeed >= 4)
    {
        turns.push_back(1);
    }

    for (int i = 0; i < turns.size(); i++)
    {
        if (turns[i] == 0)
        {
            int r1 = Util::random2();
            printf("r1 = %d\n", r1);
            if (r1 < leftHit)
            {
                int r2 = Util::random();
                printf("r2 = %d\n", r2);
                if (r2 < leftCrit)
                {
                    printf("crit\n");
                    defendingUnit->hp -= 3*leftAttack;
                }
                else
                {
                    printf("hit\n");
                    defendingUnit->hp -= leftAttack;
                }
            }
            else
            {
                printf("miss\n");
            }
        }
        else
        {
            int r1 = Util::random2();
            printf("r1 = %d\n", r1);
            if (r1 < rightHit)
            {
                int r2 = Util::random();
                printf("r2 = %d\n", r2);
                if (r2 < rightCrit)
                {
                    printf("crit\n");
                    attackingUnit->hp -= 3*rightAttack;
                }
                else
                {
                    printf("hit\n");
                    attackingUnit->hp -= rightAttack;
                }
            }
            else
            {
                printf("miss\n");
            }
        }

        if (attackingUnit->hp <= 0 || 
            defendingUnit->hp <= 0)
        {
            return;
        }
    }

    printf("\n");
}
