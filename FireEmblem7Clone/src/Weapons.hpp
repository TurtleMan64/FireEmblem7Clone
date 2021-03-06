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
    WeaponType type;
    WeaponRank rankRequirement;
    int usesMax;
    int weight;
    int might;
    int hit;
    int crit;
    int exp;
    int worth;
};
