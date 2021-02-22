#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "Sprite.hpp"
#include "WindowBox.hpp"

Sprite* WindowBox::tiles = nullptr;

void WindowBox::render(int width, int height, int x, int y)
{
    if (tiles == nullptr)
    {
        tiles = new Sprite("res/Images/Sprites/Window/Box", 0, 0, false);
    }

    if (width < 2 || height < 2)
    {
        return;
    }

    tiles->x = x;
    tiles->y = y;
    tiles->imageIndex = 0; //top left
    tiles->render();

    tiles->x = x + 16*(width - 1);
    tiles->y = y;
    tiles->imageIndex = 2; //top right
    tiles->render();

    tiles->x = x;
    tiles->y = y + 16*(height - 1);
    tiles->imageIndex = 6; //bottom left
    tiles->render();

    tiles->x = x + 16*(width  - 1);
    tiles->y = y + 16*(height - 1);
    tiles->imageIndex = 8; //bottom right
    tiles->render();

    for (int i = 1; i < width - 1; i++) //top and bottom rows
    {
        tiles->x = x + 16*(i);
        tiles->y = y;
        tiles->imageIndex = 1; //top middle
        tiles->render();

        tiles->y = y + 16*(height - 1);
        tiles->imageIndex = 7; //bottom middle
        tiles->render();
    }

    for (int i = 1; i < height - 1; i++) //left and right columns
    {
        tiles->x = x;
        tiles->y = y + 16*(i);
        tiles->imageIndex = 3; //left middle
        tiles->render();

        tiles->x = x + 16*(width - 1);
        tiles->imageIndex = 5; //right middle
        tiles->render();
    }

    for (int i = 1; i < width - 1; i++)
    {
        for (int j = 1; j < height - 1; j++)
        {
            tiles->x = x + 16*i;
            tiles->y = y + 16*j;
            tiles->imageIndex = 4; //center
            tiles->render();
        }
    }
}
