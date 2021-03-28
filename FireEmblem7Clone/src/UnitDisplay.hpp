#pragma once

#include <SDL/SDL.h>
#include <string>

#include "Unit.hpp"
#include "Weapons.hpp"
#include "Sprite.hpp"

class UnitDisplay
{
private:
    enum Page
    {
        PersonalData = 1,
        Items = 2,
        WeaponLevels = 3
    };
    static Page currentPage;

    static Sprite* background;
    static Sprite* backdropMini;
    static Sprite* backdropMugshot;
    static Sprite* backdropBattles;
    static Sprite* barPieces;
    static Sprite* weaponIcons;
    static Sprite* backdropEquipment;
    static Sprite* arrow;

    static int renderOffsetTarget;
    static int renderOffset;

    static void renderMugshot(Unit* unit, bool isEnemy);
    static void renderMini(Unit* unit, bool isEnemy);
    static void renderPagePersonalData(Unit* unit, int x, int y);
    static void renderPageItems(Unit* unit, int x, int y);
    static void renderPageWeaponLevels(Unit* unit, int x, int y);
    static void renderBar(int x, int y, int width, float progress);
    static std::string calcWeaponProgress(Unit* unit, WeaponType weapon, float* outProgress, SDL_Color* color);

public:
    static void init();
    static void step();
};
