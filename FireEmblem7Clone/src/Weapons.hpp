#pragma once

enum WeaponRank
{
    N =   0,
    E =   1,
    D =  31,
    C =  71,
    B = 121,
    A = 181,
    S = 251,
    U = 999
};

enum WeaponType
{
    NotWeapon = 0,
    Sword     = 1,
    Lance     = 2,
    Axe       = 3,
    Bow       = 4,
    Anima     = 5,
    Light     = 6,
    Dark      = 7,
    Staff     = 8
};

struct WeaponStats
{
    WeaponType type = NotWeapon;
    WeaponRank rankRequirement = N;
    int usesMax = 50;
    int weight = 0;
    int might = 0;
    int hit = 0;
    int crit = 0;
    int exp = 0;
    int worth = 0;
};
