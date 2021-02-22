#pragma once

#include <SDL/SDL.h>

#include <string>
#include <vector>

#include "MapTile.hpp"

class Sprite;

enum MapObjective
{
    Rout,
    Seize,
    Survive,
    Protect
};

class Map
{
public:
    static SDL_Texture* background;

    static int viewportX;
    static int viewportY;
    static int viewportPixelX;
    static int viewportPixelY;

    static std::vector<MapTile> tiles;
    static int tilesWidth;
    static int tilesHeight;

    static MapTile getTile(int x, int y);

    static std::vector<Unit*> unitsPlayer;
    static std::vector<Unit*> unitsEnemy;

    static Sprite* cursor;
    static int cursorX;
    static int cursorY;

    static int walkingTimer; //When this is > 0, we are waiting for walking animtation to play out.
    static std::vector<SDL_Point> walkingPath; //The tiles that you walk through
    static void generateWalkingPath(Unit* unit, int tileX, int tileY); //generate walkingPath from unit to tileX, tileY
    static bool hasTemporarilyWalked; //after walking and the menu is out.

    static Sprite* menuCursor;
    static int menuIdx;
    static std::vector<std::string> menuChoices;
    static void calculateMenuChoices();
    static void executeMenuChoice();

    static bool isViewingItems;
    static int itemIdx;
    static void renderItemWindow();

    static int itemEditIdx;
    static std::vector<std::string> itemEditChoices;
    static void renderItemEditWindow();

    static bool hasSelectedBlankTile; //when u select a blank tile on the map

    //Variables to control what "state" the map is in.

    static Unit* selectedUnit;
    static int selectedUnitOriginalTileX;
    static int selectedUnitOriginalTileY;
    static std::vector<Sprite*> previewBlueTiles;

    static Sprite* previewArrowSprite;
    static void renderPreviewArrows();

    static int turnCount;
    static int turnCountMax;

    static MapObjective objective;
    static int hudObjectiveY;
    static Sprite* hudObjectiveSprite;
    static void renderObjective();

    static int hudTileDescriptorX;
    static Sprite* hudTileDescriptorSprite;
    static void renderTileDescription();

    static int hudUnitDescriptorY;
    static Sprite* hudUnitDescriptorSprite;
    static Sprite* hudStatTexts;
    static Sprite* hudHpBar;
    static void renderUnitDecription();

    static bool tileIsBlue(Unit* unit, int tileX, int tileY);
    static void clearBlueTiles();

    static Unit* getUnitAtTile(int tileX, int tileY, std::vector<Unit*>* units);

    static std::vector<Unit*> getAdjacentUnits(Unit* unit, std::vector<Unit*>* units);

    //Calculating blue tiles
    static const int MAX_MOVEMENT = 15;
    static const int NODES_WIDTH = MAX_MOVEMENT*2 + 1;
    static const int NUM_NODES = NODES_WIDTH*NODES_WIDTH;
    static int* dijkstraGraph; //Here so that it doesnt need to malloc every time
    static int* dijkstraCost;  //Here so that it doesnt need to malloc every time
    static int* dijkstraTilesDistance; //Distance from unit to each tile
    static int* dijkstraTilesPath;     //Path from a tile to previous tile on optimal path
    static void constructGraph(Unit* unit, int* graph);
    static void calculateBlueTiles(Unit* unit);

    static void updateCursor();
    static void updateCamera();

    static Sprite* turnChangeSprite;
    static const int TURN_CHANGE_TIMER_MAX = 80;
    static int turnChangeTimer;
    static void renderTurnChange(int imageIndex);

    static bool isPhasePlayer;
    static bool isPhaseEnemy;
    static void playerPhase();
    static void enemyPhase();


public:
    static void loadFresh(int mapId);

    static void step();
};
