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
    BeepClose        =  7,
    BeepDeny         =  8,
    BeepOpen         =  9,
    Cursor           = 10,
    Whoosh           = 11,
    AttackHit        = 12,
    AttackMiss       = 13,
    AttackKill       = 14,
    AttackDieFade    = 15,
    AttackCritKill   = 16,
    LevelUpJingle    = 17,
    LevelUpDing      = 18,
    TurnChange       = 19,
    GotItem          = 20,
    ChapterTitleCard = 21
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
