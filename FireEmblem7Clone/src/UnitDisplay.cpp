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

    Unit* unit = Map::getUnitAtTile(Map::cursorX, Map::cursorY, &Map::unitsPlayer);
    if (unit == nullptr)
    {
        unit = Map::getUnitAtTile(Map::cursorX, Map::cursorY, &Map::unitsEnemy);
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
        renderOffsetTarget -= 240;
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
        renderOffsetTarget += 240;
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
    const int baseY = 16;

    switch (currentPage)
    {
        case PersonalData:
        {
            renderPagePersonalData(unit, baseX + renderOffset      , baseY);
            renderPagePersonalData(unit, baseX + renderOffset + 240, baseY+10);
            renderPagePersonalData(unit, baseX + renderOffset + 480, baseY+20);
        }
        break;

        case Items:
        {
            renderPagePersonalData(unit, baseX + renderOffset      , baseY);
            renderPagePersonalData(unit, baseX + renderOffset + 240, baseY+10);
            renderPagePersonalData(unit, baseX + renderOffset + 480, baseY+20);
        }
        break;

        case WeaponLevels:
        {
            renderPagePersonalData(unit, baseX + renderOffset      , baseY);
            renderPagePersonalData(unit, baseX + renderOffset + 240, baseY+10);
            renderPagePersonalData(unit, baseX + renderOffset + 480, baseY+20);
        }
        break;

        default:
        break;
    }

    renderMini(unit);
    renderMugshot(unit);
}

void UnitDisplay::renderMugshot(Unit* unit)
{
    backdropMugshot->render();
    SDL_Rect srcRect{(96 - 80)/2, (80 - 72)/2, 80, 72};
    SDL_Rect dstRect{13, 8, 80, 72};
    SDL_RenderCopy(Global::sdlRenderer, unit->sprMugshot->image->getTexture(0), &srcRect, &dstRect);
    Text::renderText(unit->displayName, Font::White, Text::White, 25, 81, Center, 64);
}

void UnitDisplay::renderMini(Unit* unit)
{
    backdropMini->render();
    unit->render(72, 122, 1, 0, 0);
    Text::renderText(unit->className, Font::White, Text::White, 9, 106, Left, 0);
    Text::renderText(std::to_string(unit->lvl),   Font::White, Text::Blue, 24, 121, Right, 15);
    Text::renderText(std::to_string(unit->exp),   Font::White, Text::Blue, 48, 121, Right, 15);
    Text::renderText(std::to_string(unit->hp),    Font::White, Text::Blue, 24, 137, Right, 15);
    Text::renderText(std::to_string(unit->maxHp), Font::White, Text::Blue, 48, 137, Right, 15);
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

    renderBar(x + 23, y + 14 + 15*0, (int)(20*1.5f), unit->str/20.0f);
    renderBar(x + 23, y + 14 + 15*1, (int)(20*1.5f), unit->mag/20.0f);
    renderBar(x + 23, y + 14 + 15*2, (int)(20*1.5f), unit->skl/20.0f);
    renderBar(x + 23, y + 14 + 15*3, (int)(20*1.5f), unit->spd/20.0f);
    renderBar(x + 23, y + 14 + 15*4, (int)(20*1.5f), unit->lck/20.0f);
    renderBar(x + 23, y + 14 + 15*5, (int)(20*1.5f), unit->def/20.0f);
    renderBar(x + 23, y + 14 + 15*6, (int)(20*1.5f), unit->res/20.0f);

    renderBar(x + 91, y + 14 + 15*0, (int)(13*1.5f), unit->mov/13.0f);
    renderBar(x + 91, y + 14 + 15*1, (int)(20*1.5f), unit->con/20.0f);

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

}

void UnitDisplay::renderPageWeaponLevels(Unit* unit, int x, int y)
{

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
