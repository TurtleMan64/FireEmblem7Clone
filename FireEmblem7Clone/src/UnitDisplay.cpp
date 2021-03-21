#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <string>

#include "Global.hpp"
#include "Unit.hpp"
#include "Map.hpp"
#include "Input.hpp"
#include "ClassData.hpp"
#include "WindowBox.hpp"
#include "Util.hpp"
#include "Text.hpp"
#include "Weapons.hpp"
#include "ImageAnimation.hpp"
#include "UnitDisplay.hpp"

UnitDisplay::Page UnitDisplay::currentPage = PersonalData;

int UnitDisplay::renderOffsetTarget = 0;
int UnitDisplay::renderOffset       = 0;

Sprite* UnitDisplay::background      = nullptr;
Sprite* UnitDisplay::backdropMini    = nullptr;
Sprite* UnitDisplay::backdropMugshot = nullptr;
Sprite* UnitDisplay::backdropBattles = nullptr;
Sprite* UnitDisplay::barPieces       = nullptr;
Sprite* UnitDisplay::weaponIcons     = nullptr;

void UnitDisplay::step()
{
    if (background == nullptr)
    {
        background = new Sprite("res/Images/Sprites/UnitDisplay/Background", 0, 0, false);
    }

    if (backdropMini == nullptr)
    {
        backdropMini = new Sprite("res/Images/Sprites/UnitDisplay/MiniBackdrop", 0, 104, false);
    }

    if (backdropMugshot == nullptr)
    {
        backdropMugshot = new Sprite("res/Images/Sprites/UnitDisplay/MugshotBackdrop", -3, 0, false);
    }

    if (backdropBattles == nullptr)
    {
        backdropBattles = new Sprite("res/Images/Sprites/UnitDisplay/BattlesBackdrop", 0, 0, false);
    }

    if (barPieces == nullptr)
    {
        barPieces = new Sprite("res/Images/Sprites/Bar", 0, 0, false);
    }

    if (weaponIcons == nullptr)
    {
        weaponIcons = new Sprite("res/Images/Sprites/WeaponIcon", 0, 0, false);
    }

    if (Global::frameCount % 3 == 0)
    {
        background->x--;
    }

    if (background->x < -240)
    {
        background->x += 240;
    }

    background->render();
    background->x += 240;
    background->render();
    background->x -= 240;

    bool isEnemy = false;

    Unit* unit = Map::getUnitAtTile(Map::cursorX, Map::cursorY, &Map::unitsPlayer);
    if (unit == nullptr)
    {
        unit = Map::getUnitAtTile(Map::cursorX, Map::cursorY, &Map::unitsEnemy);
        isEnemy = true;
    }

    if (Input::pressedLeft())
    {
        if (currentPage == PersonalData)
        {
            currentPage = WeaponLevels;
        }
        else
        {
            currentPage = (Page)((int)currentPage - 1);
        }
        renderOffsetTarget += 240;
    }
    else if (Input::pressedRight())
    {
        if (currentPage == WeaponLevels)
        {
            currentPage = PersonalData;
        }
        else
        {
            currentPage = (Page)((int)currentPage + 1);
        }
        renderOffsetTarget -= 240;
    }
    else if (Input::pressedUp() || Input::pressedDown())
    {
        if (unit == nullptr)
        {
            return;
        }

        std::vector<Unit*>* units = &Map::unitsPlayer;
        if (isEnemy)
        {
            units = &Map::unitsEnemy;
        }

        int currIdx = 0;
        for (int i = 0; i < units->size(); i++)
        {
            if (units->at(i) == unit)
            {
                currIdx = i;
                break;
            }
        }

        if (Input::pressedUp())
        {
            currIdx = Util::mod(currIdx - 1, (int)units->size());
        }
        else
        {
            currIdx = Util::mod(currIdx + 1, (int)units->size());
        }

        unit = units->at(currIdx);
        Map::cursorX = unit->tileX;
        Map::cursorY = unit->tileY;
    }
    else if (Input::pressedB())
    {
        Global::transitionToNewState(Global::GameState::Map, 10);
    }

    if (unit == nullptr)
    {
        return;
    }

    renderOffset = Util::approach(renderOffset, renderOffsetTarget, 0.2f);

    const int baseX = 103;
    const int baseY = 15;

    switch (currentPage)
    {
        case PersonalData:
        {
            renderPagePersonalData(unit, baseX + renderOffset      , baseY);
            renderPageItems       (unit, baseX + renderOffset + 240, baseY);
            renderPageWeaponLevels(unit, baseX + renderOffset + 480, baseY);
        }
        break;

        case Items:
        {
            renderPagePersonalData(unit, baseX + renderOffset      , baseY);
            renderPageItems       (unit, baseX + renderOffset + 240, baseY);
            renderPageWeaponLevels(unit, baseX + renderOffset + 480, baseY);
        }
        break;

        case WeaponLevels:
        {
            renderPagePersonalData(unit, baseX + renderOffset      , baseY);
            renderPageItems       (unit, baseX + renderOffset + 240, baseY);
            renderPageWeaponLevels(unit, baseX + renderOffset + 480, baseY);
        }
        break;

        default:
        break;
    }

    renderMini(unit, isEnemy);
    renderMugshot(unit, isEnemy);
}

void UnitDisplay::renderMugshot(Unit* unit, bool isEnemy)
{
    backdropMugshot->imageIndex = 0;
    if (isEnemy)
    {
        backdropMugshot->imageIndex = 1;
    }
    backdropMugshot->render();
    SDL_Rect srcRect{(96 - 80)/2, (80 - 72)/2, 80, 72};
    SDL_Rect dstRect{13, 8, 80, 72};
    SDL_RenderCopy(Global::sdlRenderer, unit->sprMugshot->image->getTexture(0), &srcRect, &dstRect);
    Text::renderText(unit->unitResources.displayName, Font::White, Text::White, 25, 81, Center, 64);
}

void UnitDisplay::renderMini(Unit* unit, bool isEnemy)
{
    backdropMini->render();
    if (isEnemy)
    {
        unit->render(72, 122, 7, 0, 0);
    }
    else
    {
        unit->render(72, 122, 1, 0, 0);
    }
    Text::renderText(unit->classResources.displayName, Font::White, Text::White, 9, 106, Left,   0);
    Text::renderText(std::to_string(unit->lvl),        Font::White, Text::Blue, 24, 121, Right, 15);
    Text::renderText(std::to_string(unit->exp),        Font::White, Text::Blue, 48, 121, Right, 15);
    Text::renderText(std::to_string(unit->hp),         Font::White, Text::Blue, 24, 137, Right, 15);
    Text::renderText(std::to_string(unit->maxHp),      Font::White, Text::Blue, 48, 137, Right, 15);
}

void UnitDisplay::renderPagePersonalData(Unit* unit, int x, int y)
{
    //while (x > 480)
    //{
    //    x = x - 720;
    //}
    x = x + 240;
    x = x % 720;
    x = x - 240;

    //while (x < -240)
    //{
    //    x = x + 720;
    //}
    x = x - 480;
    x = x % 720;
    x = x + 480;

    WindowBox::render(x, y, 17, 15);

    renderBar(x + 23, y + 14 + 15*0, (int)(unit->classResources.capStr*1.5f), unit->str/(float)unit->classResources.capStr);
    renderBar(x + 23, y + 14 + 15*1, (int)(unit->classResources.capMag*1.5f), unit->mag/(float)unit->classResources.capMag);
    renderBar(x + 23, y + 14 + 15*2, (int)(unit->classResources.capSkl*1.5f), unit->skl/(float)unit->classResources.capSkl);
    renderBar(x + 23, y + 14 + 15*3, (int)(unit->classResources.capSpd*1.5f), unit->spd/(float)unit->classResources.capSpd);
    renderBar(x + 23, y + 14 + 15*4, (int)(unit->classResources.capLck*1.5f), unit->lck/(float)unit->classResources.capLck);
    renderBar(x + 23, y + 14 + 15*5, (int)(unit->classResources.capDef*1.5f), unit->def/(float)unit->classResources.capDef);
    renderBar(x + 23, y + 14 + 15*6, (int)(unit->classResources.capRes*1.5f), unit->res/(float)unit->classResources.capRes);

    renderBar(x + 91, y + 14 + 15*0, (int)(unit->classResources.capMov*1.5f), unit->mov/(float)unit->classResources.capMov);
    renderBar(x + 91, y + 14 + 15*1, (int)(unit->classResources.capCon*1.5f), unit->con/(float)unit->classResources.capCon);

    Text::renderText("Str",   Font::White, Text::Yellow, x +  4, y + 7 + 15*0, Left, 0);
    Text::renderText("Mag",   Font::White, Text::Yellow, x +  4, y + 7 + 15*1, Left, 0);
    Text::renderText("Skill", Font::White, Text::Yellow, x +  4, y + 7 + 15*2, Left, 0);
    Text::renderText("Spd",   Font::White, Text::Yellow, x +  4, y + 7 + 15*3, Left, 0);
    Text::renderText("Luck",  Font::White, Text::Yellow, x +  4, y + 7 + 15*4, Left, 0);
    Text::renderText("Def",   Font::White, Text::Yellow, x +  4, y + 7 + 15*5, Left, 0);
    Text::renderText("Res",   Font::White, Text::Yellow, x +  4, y + 7 + 15*6, Left, 0);

    Text::renderText("Move",  Font::White, Text::Yellow, x + 72, y + 7 + 15*0, Left, 0);
    Text::renderText("Con",   Font::White, Text::Yellow, x + 72, y + 7 + 15*1, Left, 0);
    Text::renderText("Aid",   Font::White, Text::Yellow, x + 72, y + 7 + 15*2, Left, 0);
    Text::renderText("Trv",   Font::White, Text::Yellow, x + 72, y + 7 + 15*3, Left, 0);
    Text::renderText("Affin", Font::White, Text::Yellow, x + 72, y + 7 + 15*4, Left, 0);
    Text::renderText("Cond",  Font::White, Text::Yellow, x + 72, y + 7 + 15*5, Left, 0);

    Text::renderText(std::to_string(unit->str), Font::White, Text::Blue, x + 42, y + 7 + 15*0, Right, 0);
    Text::renderText(std::to_string(unit->mag), Font::White, Text::Blue, x + 42, y + 7 + 15*1, Right, 0);
    Text::renderText(std::to_string(unit->skl), Font::White, Text::Blue, x + 42, y + 7 + 15*2, Right, 0);
    Text::renderText(std::to_string(unit->spd), Font::White, Text::Blue, x + 42, y + 7 + 15*3, Right, 0);
    Text::renderText(std::to_string(unit->lck), Font::White, Text::Blue, x + 42, y + 7 + 15*4, Right, 0);
    Text::renderText(std::to_string(unit->def), Font::White, Text::Blue, x + 42, y + 7 + 15*5, Right, 0);
    Text::renderText(std::to_string(unit->res), Font::White, Text::Blue, x + 42, y + 7 + 15*6, Right, 0);

    Text::renderText(std::to_string(unit->mov), Font::White, Text::Blue, x + 110, y + 7 + 15*0, Right, 0);
    Text::renderText(std::to_string(unit->con), Font::White, Text::Blue, x + 110, y + 7 + 15*1, Right, 0);
    Text::renderText(std::to_string(unit->aid), Font::White, Text::Blue, x + 110, y + 7 + 15*2, Right, 0);

    if (unit->rescuedUnit != nullptr)
    {
        
    }
    else
    {
        Text::renderText("---", Font::White, Text::Blue, x + 110, y + 7 + 15*3, Right, 0);
    }

    //todo: affinity and condition

    backdropBattles->x = x;
    backdropBattles->y = y + 122;
    backdropBattles->render();
    Text::renderText("B", Font::White, Text::Yellow, x + 12, y + 123, Left, 0);
    Text::renderText("W", Font::White, Text::Yellow, x + 52, y + 123, Left, 0);
    Text::renderText("L", Font::White, Text::Yellow, x + 92, y + 123, Left, 0);

    Text::renderText("0", Font::White, Text::Blue, x + 12 + 10, y + 123, Left, 0);
    Text::renderText("01", Font::White, Text::Blue, x + 52 + 10, y + 123, Left, 0);
    Text::renderText("123", Font::White, Text::Blue, x + 92 + 10, y + 123, Left, 0);
}

void UnitDisplay::renderPageItems(Unit* unit, int x, int y)
{
    x = x + 240;
    x = x % 720;
    x = x - 240;

    x = x - 480;
    x = x % 720;
    x = x + 480;

    WindowBox::render(x, y, 17, 18);

    Item* equippedWeapon = unit->getEquippedWeapon();

    for (int i = 0; i < unit->items.size(); i++)
    {
        Item item = unit->items[i];

        SDL_Color colorText = Text::White;
        SDL_Color colorUses = Text::Blue;
        if (item.isWeapon())
        {
            if (!unit->canUseWeapon(item))
            {
                colorText = {180, 180, 180, 255};
                colorUses = colorText;
            }
        }
        else if (!item.isConsumableByUnit(unit))
        {
            colorText = {180, 180, 180, 255};
            colorUses = colorText;
        }

        item.render(x + 4, y + 4 + i*16);
        Text::renderText(item.getName(),                                Font::White, colorText, x + 16 + 5,      y + 5 + i*16, Left,   0);
        Text::renderText(std::to_string(item.usesRemaining),            Font::White, colorUses, x + 82,          y + 6 + i*16, Right, 16);
        Text::renderText("/",                                           Font::White, colorText, x + 82 + 18,     y + 6 + i*16, Left,   0);
        Text::renderText(std::to_string(item.getWeaponStats().usesMax), Font::White, colorUses, x + 82 + 16 + 7, y + 6 + i*16, Right, 16);

        if (equippedWeapon != nullptr &&
            equippedWeapon == &unit->items[i])
        {
            Text::renderText("E",                                       Font::White, colorText, x + 82 + 32 + 9, y + 6 + i*16, Left,   0);
        }
    }
}

void UnitDisplay::renderPageWeaponLevels(Unit* unit, int x, int y)
{
    x = x + 240;
    x = x % 720;
    x = x - 240;

    x = x - 480;
    x = x % 720;
    x = x + 480;

    WindowBox::render(x, y, 17, 15);

    weaponIcons->render(x +  4, y +  4, 0);
    weaponIcons->render(x +  4, y + 20, 1);
    weaponIcons->render(x +  4, y + 36, 2);
    weaponIcons->render(x +  4, y + 52, 3);
    weaponIcons->render(x + 69, y +  4, 4);
    weaponIcons->render(x + 69, y + 20, 5);
    weaponIcons->render(x + 69, y + 36, 6);
    weaponIcons->render(x + 69, y + 52, 7);

    std::string rank[9];
    float progress[9];

    rank[Sword] = calcWeaponProgress(unit, Sword, &progress[Sword]);
    rank[Lance] = calcWeaponProgress(unit, Lance, &progress[Lance]);
    rank[Axe  ] = calcWeaponProgress(unit, Axe  , &progress[Axe  ]);
    rank[Bow  ] = calcWeaponProgress(unit, Bow  , &progress[Bow  ]);
    rank[Anima] = calcWeaponProgress(unit, Anima, &progress[Anima]);
    rank[Light] = calcWeaponProgress(unit, Light, &progress[Light]);
    rank[Dark ] = calcWeaponProgress(unit, Dark , &progress[Dark ]);
    rank[Staff] = calcWeaponProgress(unit, Staff, &progress[Staff]);
    
    renderBar(x + 21,      y + 11,      41, progress[Sword]);
    renderBar(x + 21,      y + 11 + 16, 41, progress[Lance]);
    renderBar(x + 21,      y + 11 + 32, 41, progress[Axe  ]);
    renderBar(x + 21,      y + 11 + 48, 41, progress[Bow  ]);
    renderBar(x + 21 + 65, y + 11     , 41, progress[Anima]);
    renderBar(x + 21 + 65, y + 11 + 16, 41, progress[Light]);
    renderBar(x + 21 + 65, y + 11 + 32, 41, progress[Dark ]);
    renderBar(x + 21 + 65, y + 11 + 48, 41, progress[Staff]);

    Text::renderText(rank[Sword], Font::White, Text::Blue, x + 21,      y + 4,      Center, 41);
    Text::renderText(rank[Lance], Font::White, Text::Blue, x + 21,      y + 4 + 16, Center, 41);
    Text::renderText(rank[Axe  ], Font::White, Text::Blue, x + 21,      y + 4 + 32, Center, 41);
    Text::renderText(rank[Bow  ], Font::White, Text::Blue, x + 21,      y + 4 + 48, Center, 41);
    Text::renderText(rank[Anima], Font::White, Text::Blue, x + 21 + 65, y + 4,      Center, 41);
    Text::renderText(rank[Light], Font::White, Text::Blue, x + 21 + 65, y + 4 + 16, Center, 41);
    Text::renderText(rank[Dark ], Font::White, Text::Blue, x + 21 + 65, y + 4 + 32, Center, 41);
    Text::renderText(rank[Staff], Font::White, Text::Blue, x + 21 + 65, y + 4 + 48, Center, 41);
}

void UnitDisplay::renderBar(int x, int y, int width, float progress)
{
    //barPieces
    if (width < 5)
    {
        return;
    }

    //caps
    SDL_Rect dstRect{x, y, 1, 5};
    SDL_RenderCopy(Global::sdlRenderer, barPieces->image->getTexture(0), nullptr, &dstRect);
    dstRect.x = (x + width) - 1;
    SDL_RenderCopy(Global::sdlRenderer, barPieces->image->getTexture(0), nullptr, &dstRect);

    //bg
    dstRect.x = x + 1;
    dstRect.w = width - 2;
    SDL_RenderCopy(Global::sdlRenderer, barPieces->image->getTexture(1), nullptr, &dstRect);

    //fill
    dstRect.x = x + 2;
    dstRect.w = (int)((width - 4)*progress);
    SDL_RenderCopy(Global::sdlRenderer, barPieces->image->getTexture(2), nullptr, &dstRect);
}

std::string UnitDisplay::calcWeaponProgress(Unit* unit, WeaponType weapon, float* outProgress)
{
    int rank = unit->weaponRank[weapon];

    if (rank < E)
    {
        *outProgress = 0;
        return "-";
    }
    else if (rank < D)
    {
        *outProgress = (rank - E)/((float)(D - E));
        return "E";
    }
    else if (rank < C)
    {
        *outProgress = (rank - D)/((float)(C - D));
        return "D";
    }
    else if (rank < B)
    {
        *outProgress = (rank - C)/((float)(B - C));
        return "C";
    }
    else if (rank < A)
    {
        *outProgress = (rank - B)/((float)(A - B));
        return "B";
    }
    else if (rank < S)
    {
        *outProgress = (rank - A)/((float)(S - A));
        return "A";
    }
    else
    {
        *outProgress = 0;
        return "S";
    }
}
