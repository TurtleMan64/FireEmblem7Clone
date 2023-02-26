#pragma once

#include <SDL/SDL_mixer.h>

#include <vector>

enum SfxId
{
    Beep1            =  0,
    Beep2            =  1,
    Beep3            =  2,
    Beep4            =  3,
    Beep5            =  4,
    Beep6            =  5,
    Beep7            =  6,
    Beep8            =  7,
    BeepClose        =  8,
    BeepDeny         =  9,
    BeepOpen         = 10,
    Cursor           = 11,
    Whoosh           = 12,
    AttackHit        = 13,
    AttackMiss       = 14,
    AttackKill       = 15,
    AttackDieFade    = 16,
    AttackCritKill   = 17,
    LevelUpJingle    = 18,
    LevelUpDing      = 19,
    TurnChange       = 20,
    GotItem          = 21,
    ChapterTitleCard = 22
};

class Audio
{
private:
    static std::vector<Mix_Chunk*> sfx;

    static void loadSFX(std::string path);

public:
    static void init();

    static void play(SfxId id, int channel);
};
