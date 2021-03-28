#pragma once

class Sprite;

class WindowBox
{
private:
    static Sprite* tiles;
    
public:
    static void init();
    static void render(int x, int y, int width, int height);
};
