#include <SDL/SDL_mixer.h>

#include "Audio.hpp"

std::vector<Mix_Chunk*> Audio::sfx;

void Audio::init()
{
    Mix_OpenAudio(44100, AUDIO_S16LSB, 4, 512);

    loadSFX("res/Audio/SFX/Beep1.wav");
    loadSFX("res/Audio/SFX/Beep2.wav");
    loadSFX("res/Audio/SFX/Beep3.wav");
    loadSFX("res/Audio/SFX/Beep4.wav");
    loadSFX("res/Audio/SFX/Beep5.wav");
    loadSFX("res/Audio/SFX/Beep6.wav");
    loadSFX("res/Audio/SFX/Beep7.wav");
    loadSFX("res/Audio/SFX/Beep8.wav");
    loadSFX("res/Audio/SFX/BeepClose.wav");
    loadSFX("res/Audio/SFX/BeepDeny.wav");
    loadSFX("res/Audio/SFX/BeepOpen.wav");
    loadSFX("res/Audio/SFX/Cursor.wav");
    loadSFX("res/Audio/SFX/Whoosh.wav");
    loadSFX("res/Audio/SFX/AttackHit.wav");
    loadSFX("res/Audio/SFX/AttackMiss.wav");
    loadSFX("res/Audio/SFX/AttackKill.wav");
    loadSFX("res/Audio/SFX/AttackDieFade.wav");
    loadSFX("res/Audio/SFX/AttackCritKill.wav");
    loadSFX("res/Audio/SFX/LevelUpJingle.wav");
    loadSFX("res/Audio/SFX/LevelUpDing.wav");
    loadSFX("res/Audio/SFX/TurnChange.wav");
    loadSFX("res/Audio/SFX/GotItem.wav");
    loadSFX("res/Audio/SFX/ChapterTitleCard.wav");
}

void Audio::play(SfxId id, int channel)
{
    Mix_PlayChannel(channel, sfx[(int)id], 0);
}

void Audio::loadSFX(std::string path)
{
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (chunk == nullptr)
    {
        printf("Error loading sound effect '%s'\n", path.c_str());
    }
    sfx.push_back(chunk);
}
