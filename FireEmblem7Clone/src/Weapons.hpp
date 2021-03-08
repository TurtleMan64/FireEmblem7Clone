#pragma once

enum WeaponRank
{
    N = 0,
    E = 1,
    D = 2,
    C = 3,
    B = 4,
    A = 5,
    S = 6,
    U = 7
};

enum WeaponType
{
    NotWeapon = 0,
    Sword = 1,
    Lance = 2,
    Axe   = 3,
    Bow   = 4,
    Anima = 5,
    Light = 6,
    Dark  = 7,
    Staff = 8
};

struct WeaponStats
{
    WeaponType type = NotWeapon;
    WeaponRank rankRequirement = N;
    int usesMax = 1;
    int weight = 0;
    int might = 0;
    int hit = 0;
    int crit = 0;
    int exp = 0;
    int worth = 0;
};
