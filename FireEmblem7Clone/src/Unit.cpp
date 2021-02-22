#include <SDL/SDL.h>

#include <string>
#include <vector>

#include "Weapons.hpp"
#include "Items.hpp"
#include "Global.hpp"
#include "Sprite.hpp"
#include "Util.hpp"
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
    sprMapRunUpB    = new Sprite(resources.mapPath + "/RunUp",   0, 0, false);
    sprMapRunDownB  = new Sprite(resources.mapPath + "/RunDown", 0, 0, false);
    sprMapRunLeftB  = new Sprite(resources.mapPath + "/RunLeft", 0, 0, false);
    sprMapHuzzahB   = new Sprite(resources.mapPath + "/Huzzah",  0, 0, false);
    sprMapIdleR     = new Sprite(resources.mapPath + "/Idle",    0, 0, true);
    sprMapRunUpR    = new Sprite(resources.mapPath + "/RunUp",   0, 0, true);
    sprMapRunDownR  = new Sprite(resources.mapPath + "/RunDown", 0, 0, true);
    sprMapRunLeftR  = new Sprite(resources.mapPath + "/RunLeft", 0, 0, true);
    sprMapHuzzahR   = new Sprite(resources.mapPath + "/Huzzah",  0, 0, true);
    sprMugshot      = new Sprite(resources.mugshotPath,          0, 0, false);
    sprMugshotTiny  = new Sprite(resources.mugshotTinyPath,      0, 0, false);

    mov = 9;
}

Unit::~Unit()
{
    delete sprMapIdleR;
    delete sprMapRunUpR;
    delete sprMapRunDownR;
    delete sprMapRunLeftR;
    delete sprMapIdleB;
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
        case  1: s = sprMapRunUpB;   break;
        case  2: s = sprMapRunDownB; break;
        case  3: s = sprMapRunLeftB; sprMapRunLeftB->scaleX =  1; break;
        case  4: s = sprMapRunLeftB; sprMapRunLeftB->scaleX = -1; break;
        case  5: s = sprMapIdleR;    break;
        case  6: s = sprMapRunUpR;   break;
        case  7: s = sprMapRunDownR; break;
        case  8: s = sprMapRunLeftR; sprMapRunLeftR->scaleX =  1; break;
        case  9: s = sprMapRunLeftR; sprMapRunLeftR->scaleX = -1; break;
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
            WeaponType weaponType = item.getWeaponType();
            WeaponRank weaponRankRequirement = item.getWeaponRankRequirement();

            if (weaponRank[weaponType] >= weaponRankRequirement)
            {
                std::unordered_set<int> range = item.getWeaponRange();

                ranges.insert(range.begin(), range.end());
            }
        }
    }

    return ranges;
}
