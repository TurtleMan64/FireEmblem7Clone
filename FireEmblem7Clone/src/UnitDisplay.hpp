#pragma once

#include <SDL/SDL.h>

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

    static int renderOffsetTarget;
    static int renderOffset;

    static void renderMugshot(Unit* unit);
    static void renderMini(Unit* unit);
    static void renderPagePersonalData(Unit* unit, int x, int y);
    static void renderPageItems(Unit* unit, int x, int y);
    static void renderPageWeaponLevels(Unit* unit, int x, int y);
    static void renderBar(int x, int y, int width, float progress);

public:
    static void step();
};
