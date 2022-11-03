#include <vector>

#include "Unit.hpp"
#include "Util.hpp"
#include "Weapons.hpp"
#include "Items.hpp"
#include "Battle.hpp"

std::vector<TurnResult> Battle::results;

void Battle::doBattle(Unit* initialAttackingUnit, Unit* initialDefendingUnit)
{
    std::vector<int> turns; //0 = lefts turn, 1 = rights turn

    results.clear();

    int leftAttackSpeed = initialAttackingUnit->spd;
    Item* leftWeapon = initialAttackingUnit->getEquippedWeapon();
    if (leftWeapon != nullptr)
    {
        leftAttackSpeed = initialAttackingUnit->getAttackSpeedWithWeapon(leftWeapon->getWeaponStats());
    }
            
    int rightAttackSpeed = initialDefendingUnit->spd;
    Item* rightWeapon = initialDefendingUnit->getEquippedWeapon();
    if (rightWeapon != nullptr)
    {
        rightAttackSpeed = initialDefendingUnit->getAttackSpeedWithWeapon(rightWeapon->getWeaponStats());
    }

    int leftAttack  = 0;
    int leftHit     = 0;
    int leftCrit    = 0;
    int rightAttack = 0;
    int rightHit    = 0;
    int rightCrit   = 0;
    initialAttackingUnit->calculateCombatStatsVsUnit(initialDefendingUnit, &leftAttack,  &leftHit,  &leftCrit, &rightAttack, &rightHit, &rightCrit);

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

    TurnResult initialTurn;

    initialTurn.unitLeft  = initialDefendingUnit;
    initialTurn.unitRight = initialAttackingUnit;
    initialTurn.unitAttacking = initialAttackingUnit;
    initialTurn.unitDefending = initialDefendingUnit;

    initialTurn.unitLeftHp  = initialDefendingUnit->hp;
    initialTurn.unitLeftAtk = leftAttack;
    initialTurn.unitLeftHit = leftHit;
    initialTurn.unitLeftCrt = leftCrit;

    initialTurn.unitRightHp  = initialAttackingUnit->hp;
    initialTurn.unitRightAtk = rightAttack;
    initialTurn.unitRightHit = rightHit;
    initialTurn.unitRightCrt = rightCrit;

    if (leftWeapon != nullptr)
    {
        initialTurn.leftWeapon = *leftWeapon;
    }
    if (rightWeapon != nullptr)
    {
        initialTurn.rightWeapon = *rightWeapon;
    }

    results.push_back(initialTurn);

    for (int i = 0; i < turns.size(); i++)
    {
        //Check if the attacking unit's weapon still has uses left
        if (turns[i] == 0 && leftWeapon != nullptr && leftWeapon->usesRemaining <= 0)
        {
            continue;
        }

        if (turns[i] == 1 && rightWeapon != nullptr && rightWeapon->usesRemaining <= 0)
        {
            continue;
        }

        //Check if the attacking unit has a weapon
        if (turns[i] == 0 && leftWeapon == nullptr)
        {
            continue;
        }

        if (turns[i] == 1 && rightWeapon == nullptr)
        {
            continue;
        }

        //Checking if the hit is > 0
        if (turns[i] == 0 && leftHit <= 0)
        {
            continue;
        }

        if (turns[i] == 1 && rightHit <= 0)
        {
            continue;
        }

        TurnResult turn;
        turn.hit  = false;
        turn.crit = false;
        turn.unitLeft    = initialDefendingUnit;
        turn.unitRight   = initialAttackingUnit;
        turn.unitLeftAtk = leftAttack;
        turn.unitLeftHit = leftHit;
        turn.unitLeftCrt = leftCrit;
        turn.unitRightAtk = rightAttack;
        turn.unitRightHit = rightHit;
        turn.unitRightCrt = rightCrit;
        if (leftWeapon != nullptr)
        {
            turn.leftWeapon = *leftWeapon;
        }
        if (rightWeapon != nullptr)
        {
            turn.rightWeapon = *rightWeapon;
        }

        if (turns[i] == 0)
        {
            turn.unitAttacking = initialAttackingUnit;
            turn.unitDefending = initialDefendingUnit;

            int r1 = Util::random2();
            if (r1 < leftHit)
            {
                int r2 = Util::random();
                if (r2 < leftCrit)
                {
                    turn.unitDefending->hp -= 3*leftAttack;

                    turn.crit = true;
                    turn.hit = true;
                }
                else
                {
                    turn.unitDefending->hp -= leftAttack;
                    turn.hit = true;
                }
                leftWeapon->usesRemaining--;
            }
        }
        else
        {
            turn.unitAttacking = initialDefendingUnit;
            turn.unitDefending = initialAttackingUnit;

            int r1 = Util::random2();
            if (r1 < rightHit)
            {
                int r2 = Util::random();
                if (r2 < rightCrit)
                {
                    turn.unitDefending->hp -= 3*rightAttack;
                    turn.crit = true;
                    turn.hit = true;
                }
                else
                {
                    turn.unitDefending->hp -= rightAttack;
                    turn.hit = true;
                }
                rightWeapon->usesRemaining--;
            }
        }

        initialAttackingUnit->hp = Util::max(0, initialAttackingUnit->hp);
        initialDefendingUnit->hp = Util::max(0, initialDefendingUnit->hp);

        turn.unitLeftHp  = initialDefendingUnit->hp;
        turn.unitRightHp = initialAttackingUnit->hp;

        results.push_back(turn);

        if (initialAttackingUnit->hp <= 0 ||
            initialDefendingUnit->hp <= 0)
        {
            return;
        }
    }
}
