#include <vector>

#include "Unit.hpp"
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


}
