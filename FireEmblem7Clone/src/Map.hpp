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

enum MapState
{
    Neutral,
    NeutralMenu,
    MovingUnit,
    WaitingForUnitToMove,
    UnitMenu,
    UnitMenuItem,
    UnitMenuItemAction,
    UnitMenuAttackSelectTarget,
    UnitMenuTradeSelectTarget,
    UnitMenuTrading,
    WaitingForAttackToFinish,
    PlayerPhaseEnding,
    EnemyPhaseEnding
};

class Map
{
public:
    static SDL_Texture* background;

    static MapState mapState;

    static int viewportX;
    static int viewportY;
    static int viewportPixelX;
    static int viewportPixelY;

    static std::vector<MapTile> tiles;
    static int tilesWidth;
    static int tilesHeight;

    static MapTile getTile(int x, int y, std::vector<Unit*>* unpassableUnits);

    static std::vector<Unit*> unitsPlayer;
    static std::vector<Unit*> unitsEnemy;

    static void renderUnits(std::vector<Unit*>* units, int spriteIndex, Unit* ignoreMe);

    static Sprite* cursor;
    static int cursorX;
    static int cursorY;
    static void renderCursor();

    static int walkingTimer; //When this is > 0, we are waiting for walking animtation to play out.
    static std::vector<SDL_Point> walkingPath; //The tiles that you walk through
    //static void generateWalkingPath(Unit* unit, int tileX, int tileY); //generate walkingPath from unit to tileX, tileY
    //static bool hasTemporarilyWalked; //after walking and the menu is out.

    static Sprite* menuCursor;
    static int menuIdx;
    static std::vector<std::string> menuChoices;
    static void calculateMenuChoices();
    static void executeMenuChoice();

    static void renderMainMenu();
    static void renderHandCursor(int windowX, int windowY, int idx);

    //static bool isViewingItems;
    static int itemIdx;
    static void renderItemWindow(Unit* unit, int x, int y, int windowHeight, bool greyUnusables);

    static void renderItemWeaponStatsWindow();

    static int itemEditIdx;
    static std::vector<std::string> itemEditChoices;
    static void renderItemEditWindow();

    //static bool hasSelectedBlankTile; //when u select a blank tile on the map

    static Sprite* attackPreviewBackdrop;
    static Sprite* attackPreviewMultiplier;
    static void renderAttackPreview();

    static Sprite* attackingHealthDisplay;
    static Sprite* attackingMiss;
    static Unit* defendingEnemy;
    static int attackingTimer;

    static Unit* selectedUnit;
    static int selectedUnitOriginalTileX;
    static int selectedUnitOriginalTileY;
    static bool selectedUnitCanGoBack;

    static std::vector<Sprite*> previewTilesBlue;
    static std::vector<Sprite*> previewTilesRed;

    static std::vector<Sprite*> previewTilesEnemyAll; //Preview for all enemies

    static void createAttackPreviewTiles(Unit* unit, std::unordered_set<int> ranges);
    static std::unordered_set<int> calculateRedTilesAtTile(int tileX, int tileY, std::unordered_set<int> ranges);

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

    static void resetNeutralHudDescriptions();

    //static bool unitCanMoveToTile(Unit* unit, int tileX, int tileY);
    static void clearPreviewTiles();
    static void clearPreviewTilesEnemyAll();
    static void renderPreviewTiles();
    static void renderPreviewTilesEnemyAll();

    static Unit* getUnitAtTile(int tileX, int tileY, std::vector<Unit*>* units);

    static std::vector<Unit*> getAdjacentUnits(Unit* unit, std::vector<Unit*>* units);

    static std::vector<Unit*> getEnemiesInRedTiles();

    static int tradeLeftIdx;
    static int tradeRightIdx;
    static bool tradeSideLeft;
    static bool tradeIsLocked;
    static Unit* tradingUnit;

    //Calculating blue tiles
    //static const int MAX_MOVEMENT = 15;
    //static const int NODES_WIDTH = MAX_MOVEMENT*2 + 1;
    //static const int NUM_NODES = NODES_WIDTH*NODES_WIDTH;
    //static int* dijkstraGraph; //Here so that it doesnt need to malloc every time
    //static int* dijkstraCost;  //Here so that it doesnt need to malloc every time
    //static int* dijkstraTilesDistance; //Distance from unit to each tile
    //static int* dijkstraTilesPath;     //Path from a tile to previous tile on optimal path
    //static void constructGraph(Unit* unit, int* graph);
    //static void calculatePreviewTiles(Unit* unit);

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
    static void init();

    static void loadFresh(int mapId);

    static void step();
};
