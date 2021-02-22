#pragma once

class Sprite;

class WindowBox
{
private:
    static Sprite* tiles;
    
public:
    static void render(int width, int height, int x, int y);
};
