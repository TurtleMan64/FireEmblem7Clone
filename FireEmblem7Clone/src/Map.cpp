#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <string>
#include <vector>
#include <unordered_set>
#include <math.h>

#include "Global.hpp"
#include "Sprite.hpp"
#include "Battle.hpp"
#include "Util.hpp"
#include "Input.hpp"
#include "Items.hpp"
#include "Weapons.hpp"
#include "WindowBox.hpp"
#include "Text.hpp"
#include "MapTile.hpp"
#include "Djikstra.hpp"
#include "Map.hpp"
#include "Audio.hpp"


SDL_Texture* Map::background = nullptr;
std::vector<MapTile> Map::tiles;
int Map::tilesWidth  = 17;
int Map::tilesHeight = 16;

std::vector<Unit*> Map::unitsPlayer;
std::vector<Unit*> Map::unitsEnemy;

MapState Map::mapState = Neutral;

int Map::viewportX = 0;
int Map::viewportY = 0;
int Map::viewportPixelX = 0;
int Map::viewportPixelY = 0;

Sprite* Map::cursor = nullptr;
int Map::cursorX = 0;
int Map::cursorY = 0;

int Map::walkingTimer = 0;
std::vector<SDL_Point> Map::walkingPath;

Sprite* Map::menuCursor = nullptr;
int Map::menuIdx = 0;
std::vector<std::string> Map::menuChoices;

int  Map::itemIdx = 0;

int  Map::itemEditIdx = 0;
std::vector<std::string> Map::itemEditChoices;

int Map::staffIdx;
std::vector<Item*> Map::staffChoices;
std::vector<Sprite*> Map::previewTilesGreen;


Sprite* Map::attackPreviewBackdrop = nullptr;
Sprite* Map::attackPreviewMultiplier = nullptr;

Sprite* Map::turnChangeSprite = nullptr;
int Map::turnChangeTimer = 0;
bool Map::isPhasePlayer = true;
bool Map::isPhaseEnemy = true;

Unit* Map::selectedUnit = nullptr;
int Map::selectedUnitOriginalTileX = 0;
int Map::selectedUnitOriginalTileY = 0;
bool Map::selectedUnitCanGoBack = true;

std::vector<Sprite*> Map::previewTilesBlue;
std::vector<Sprite*> Map::previewTilesRed;

std::vector<Sprite*> Map::previewTilesEnemyAll; //Preview for all enemies

Sprite* Map::previewArrowSprite = nullptr;

int Map::turnCount = 0;
int Map::turnCountMax = 99;

MapObjective Map::objective = Rout;
SDL_Point Map::seizeTile = {0, 0};
int Map::hudObjectiveY = 0;
Sprite* Map::hudObjectiveSprite = nullptr;

int Map::hudTileDescriptorX = 0;
Sprite* Map::hudTileDescriptorSprite = nullptr;

int Map::hudUnitDescriptorY = 0;
Sprite* Map::hudUnitDescriptorSprite = nullptr;
Sprite* Map::hudStatTexts = nullptr;
Sprite* Map::hudHpBar = nullptr;

int  Map::tradeLeftIdx  = 0;
int  Map::tradeRightIdx = 0;
bool Map::tradeSideLeft = false;
bool Map::tradeIsLocked = false;
Unit* Map::tradingUnit = nullptr;

Sprite* Map::attackingMiss = nullptr;
Sprite* Map::attackingHealthDisplay = nullptr;
Unit* Map::defendingEnemy = nullptr;
int Map::attackingTimer = 0;

int Map::enemyIdx = 0;
int Map::enemyWalkingTimer = 0;
std::vector<SDL_Point> Map::enemyWalkingPath;
SDL_Point Map::enemyTileToAttackFrom;
Unit* Map::unitEnemyWillAttack = nullptr;
Item Map::weaponEnemyAttacksWith;
int Map::enemyAttackingTimer = 0;
MapState Map::enemyStateAfterCurrentState = EnemyPhaseCalculating;

void Map::init()
{
    cursor                  = new Sprite("res/Images/Sprites/Map/Cursor",              0,  0, false);
    previewArrowSprite      = new Sprite("res/Images/Sprites/Map/PreviewArrow",        0,  0, false);
    menuCursor              = new Sprite("res/Images/Sprites/Window/Hand",             0,  0, false);
    hudObjectiveSprite      = new Sprite("res/Images/Sprites/Map/ObjectiveDisplay",    0,  0, false);
    hudTileDescriptorSprite = new Sprite("res/Images/Sprites/Map/TileDisplay",         0,  0, false);
    hudUnitDescriptorSprite = new Sprite("res/Images/Sprites/Map/UnitDisplay",         0,  0, false);
    hudStatTexts            = new Sprite("res/Images/Sprites/Map/StatTexts",           0,  0, false);
    hudHpBar                = new Sprite("res/Images/Sprites/Map/HpBar",               0,  0, false);
    turnChangeSprite        = new Sprite("res/Images/Sprites/Map/TurnChange",          0, 64, false);
    attackPreviewBackdrop   = new Sprite("res/Images/Sprites/Map/PreviewAttack",       0,  0, false);
    attackPreviewMultiplier = new Sprite("res/Images/Sprites/Map/Multipliers",         0,  0, false);
    attackingMiss           = new Sprite("res/Images/Sprites/Map/Miss",                0,  0, false);
    attackingHealthDisplay  = new Sprite("res/Images/Sprites/Map/AttackHealthDisplay", 0,  0, false);
}

void Map::loadFresh(int mapId)
{
    Global::mapId = mapId;
    std::vector<std::string> mapFile = Util::readFile("res/Maps/Chapter" + std::to_string(mapId) + ".map");

    int lineNum = 0;

    background = IMG_LoadTexture(Global::sdlRenderer, mapFile[lineNum].c_str()); lineNum++;

    std::string currObjective = mapFile[lineNum]; lineNum++;
    if (currObjective == "Rout"   ) { objective = Rout   ; }
    if (currObjective == "Seize"  ) { objective = Seize  ; }
    if (currObjective == "Survive") { objective = Survive; }
    if (currObjective == "Protect") { objective = Protect; }

    tilesWidth  = std::stoi(mapFile[lineNum]); lineNum++;
    tilesHeight = std::stoi(mapFile[lineNum]); lineNum++;

    tiles.clear();
    for (int y = 0; y < tilesHeight; y++)
    {
        std::vector<std::string> rowTiles = Util::split(mapFile[lineNum], ' '); lineNum++;
        for (int x = 0; x < tilesWidth; x++)
        {
            char tileChar = (rowTiles[x])[0];
            MapTile tile((MapTileType)tileChar, "test" + std::to_string(tileChar));
            tiles.push_back(tile);
        }
    }

    seizeTile.x = std::stoi(mapFile[lineNum]); lineNum++;
    seizeTile.y = std::stoi(mapFile[lineNum]); lineNum++;

    lineNum++;

    int numPlayers = std::stoi(mapFile[lineNum]); lineNum++;
    for (int i = 0; i < numPlayers; i++)
    {
        std::vector<std::string> line = Util::split(mapFile[lineNum], ' '); lineNum++;
        Unit* player = new Unit(line[0], line[1]);

        player->tileX = std::stoi(line[ 2]);
        player->tileY = std::stoi(line[ 3]);
        player->lvl   = std::stoi(line[ 4]);
        player->hp    = std::stoi(line[ 5]);
        player->maxHp = std::stoi(line[ 5]);
        player->str   = std::stoi(line[ 6]);
        player->mag   = std::stoi(line[ 7]);
        player->skl   = std::stoi(line[ 8]);
        player->spd   = std::stoi(line[ 9]);
        player->lck   = std::stoi(line[10]);
        player->def   = std::stoi(line[11]);
        player->res   = std::stoi(line[12]);
        player->mov   = std::stoi(line[13]);
        player->con   = std::stoi(line[14]);
        player->aid   = std::stoi(line[15]);

        Item item1 = Item((ItemId)std::stoi(line[16]));
        Item item2 = Item((ItemId)std::stoi(line[17]));
        Item item3 = Item((ItemId)std::stoi(line[18]));
        Item item4 = Item((ItemId)std::stoi(line[19]));
        Item item5 = Item((ItemId)std::stoi(line[20]));

        if (item1.id != None) { player->items.push_back(item1); }
        if (item2.id != None) { player->items.push_back(item2); }
        if (item3.id != None) { player->items.push_back(item3); }
        if (item4.id != None) { player->items.push_back(item4); }
        if (item5.id != None) { player->items.push_back(item5); }

        player->weaponRank[Sword] = std::stoi(line[21]);
        player->weaponRank[Lance] = std::stoi(line[22]);
        player->weaponRank[Axe  ] = std::stoi(line[23]);
        player->weaponRank[Bow  ] = std::stoi(line[24]);
        player->weaponRank[Anima] = std::stoi(line[25]);
        player->weaponRank[Light] = std::stoi(line[26]);
        player->weaponRank[Dark ] = std::stoi(line[27]);
        player->weaponRank[Staff] = std::stoi(line[28]);

        unitsPlayer.push_back(player);
    }

    lineNum++;

    int numEnemies = std::stoi(mapFile[lineNum]); lineNum++;
    for (int i = 0; i < numEnemies; i++)
    {
        std::vector<std::string> line = Util::split(mapFile[lineNum], ' '); lineNum++;
        Unit* enemy = new Unit(line[0], line[1]);

        enemy->tileX = std::stoi(line[ 2]);
        enemy->tileY = std::stoi(line[ 3]);
        enemy->lvl   = std::stoi(line[ 4]);
        enemy->hp    = std::stoi(line[ 5]);
        enemy->maxHp = std::stoi(line[ 5]);
        enemy->str   = std::stoi(line[ 6]);
        enemy->mag   = std::stoi(line[ 7]);
        enemy->skl   = std::stoi(line[ 8]);
        enemy->spd   = std::stoi(line[ 9]);
        enemy->lck   = std::stoi(line[10]);
        enemy->def   = std::stoi(line[11]);
        enemy->res   = std::stoi(line[12]);
        enemy->mov   = std::stoi(line[13]);
        enemy->con   = std::stoi(line[14]);
        enemy->aid   = std::stoi(line[15]);

        Item item1 = Item((ItemId)std::stoi(line[16]));
        Item item2 = Item((ItemId)std::stoi(line[17]));
        Item item3 = Item((ItemId)std::stoi(line[18]));
        Item item4 = Item((ItemId)std::stoi(line[19]));
        Item item5 = Item((ItemId)std::stoi(line[20]));

        if (item1.id != None) { enemy->items.push_back(item1); }
        if (item2.id != None) { enemy->items.push_back(item2); }
        if (item3.id != None) { enemy->items.push_back(item3); }
        if (item4.id != None) { enemy->items.push_back(item4); }
        if (item5.id != None) { enemy->items.push_back(item5); }

        enemy->weaponRank[Sword] = std::stoi(line[21]);
        enemy->weaponRank[Lance] = std::stoi(line[22]);
        enemy->weaponRank[Axe  ] = std::stoi(line[23]);
        enemy->weaponRank[Bow  ] = std::stoi(line[24]);
        enemy->weaponRank[Anima] = std::stoi(line[25]);
        enemy->weaponRank[Light] = std::stoi(line[26]);
        enemy->weaponRank[Dark ] = std::stoi(line[27]);
        enemy->weaponRank[Staff] = std::stoi(line[28]);

        unitsEnemy.push_back(enemy);
    }
}

void Map::step()
{
    updateCamera();

    SDL_Rect rect;
    rect.x = viewportPixelX;
    rect.y = viewportPixelY;
    rect.w = tilesWidth*16;
    rect.h = tilesHeight*16;
    SDL_RenderCopy(Global::sdlRenderer, background, nullptr, &rect);

    if (isPhasePlayer)
    {
        playerPhase();
    }
    else
    {
        enemyPhase();
    }
}

void Map::playerPhase()
{
    switch (mapState)
    {
    case Neutral:
    {
        updateCursor();

        Unit* useableUnitOnCursor = getUnitAtTile(cursorX, cursorY, &unitsPlayer);
        if (useableUnitOnCursor != nullptr)
        {
            if (useableUnitOnCursor->isUsed)
            {
                useableUnitOnCursor = nullptr;
            }
        }

        Unit* enemyUnitOnCursor = getUnitAtTile(cursorX, cursorY, &unitsEnemy);
        if (enemyUnitOnCursor != nullptr)
        {
            if (enemyUnitOnCursor->isUsed)
            {
                enemyUnitOnCursor = nullptr;
            }
        }

        renderPreviewTilesEnemyAll();
        renderPreviewTiles();

        renderUnits(&unitsEnemy,  6, nullptr);
        renderUnits(&unitsPlayer, 0, useableUnitOnCursor);
        if (useableUnitOnCursor != nullptr)
        {
            useableUnitOnCursor->render(useableUnitOnCursor->tileX*16, useableUnitOnCursor->tileY*16, 1, viewportPixelX, viewportPixelY);
        }

        bool allDone = true;
        for (int i = 0; i < unitsPlayer.size(); i++)
        {
            if (!unitsPlayer[i]->isUsed)
            {
                allDone = false;
                break;
            }
        }

        if (allDone)
        {
            turnChangeTimer = TURN_CHANGE_TIMER_MAX;
            mapState = PlayerPhaseEnding;
            break;
        }

        renderCursor();
        renderTileDescription();
        renderObjective();
        renderUnitDecription();

        if (Input::pressedA())
        {
            if (useableUnitOnCursor != nullptr)
            {
                Audio::play(Beep2, 0);
                selectedUnit = useableUnitOnCursor;
                selectedUnitOriginalTileX = selectedUnit->tileX;
                selectedUnitOriginalTileY = selectedUnit->tileY;
                walkingPath.clear();
                clearPreviewTiles();
                Djikstra::calculatePreviewTiles(selectedUnit, &previewTilesBlue, &previewTilesRed, &unitsPlayer, &unitsEnemy);
                mapState = MovingUnit;
                selectedUnitCanGoBack = true;
            }
            else if (enemyUnitOnCursor != nullptr)
            {
                Audio::play(Beep3, 0);
                walkingPath.clear();
                clearPreviewTiles();
                Djikstra::calculatePreviewTiles(enemyUnitOnCursor, &previewTilesBlue, &previewTilesRed, &unitsEnemy, &unitsPlayer);
            }
            else
            {
                Audio::play(Beep3, 0);
                menuChoices.clear();
                menuIdx = 0;
                menuChoices.push_back("Unit");
                menuChoices.push_back("Status");
                menuChoices.push_back("Options");
                menuChoices.push_back("Suspend");
                menuChoices.push_back("End");
                mapState = NeutralMenu;
            }
        }
        else if (Input::pressedB())
        {
            if (previewTilesBlue.size() > 0 || previewTilesRed.size() > 0)
            {
                Audio::play(Beep1, 0);
                clearPreviewTiles();
            }
        }
        else if (Input::pressedY())
        {
            if (getUnitAtTile(cursorX, cursorY, &unitsPlayer) != nullptr || getUnitAtTile(cursorX, cursorY, &unitsEnemy) != nullptr)
            {
                Audio::play(Beep4, 0);
                Global::transitionToNewState(Global::GameState::UnitDisplay, 10);
            }
            else
            {
                if (previewTilesEnemyAll.size() > 0)
                {
                    Audio::play(Beep1, 0);
                    clearPreviewTilesEnemyAll();
                }
                else
                {
                    std::unordered_set<int> allTiles;
                    std::vector<Sprite*> blues;
                    std::vector<Sprite*> reds;

                    for (int i = 0; i < unitsEnemy.size(); i++)
                    {
                        Djikstra::calculatePreviewTiles(unitsEnemy[i], &blues, &reds, &unitsPlayer, &unitsEnemy);

                        for (int t = 0; t < blues.size(); t++)
                        {
                            Sprite* s = blues[t];
                            allTiles.insert(s->x | (s->y << 16));
                            delete s;
                        }
                        blues.clear();

                        for (int t = 0; t < reds.size(); t++)
                        {
                            Sprite* s = reds[t];
                            allTiles.insert(s->x | (s->y << 16));
                            delete s;
                        }
                        reds.clear();
                    }

                    for (auto itr = allTiles.begin(); itr != allTiles.end(); ++itr)
                    {
                        int pos = *itr;
                        int x = (pos >>  0) & 0xFFFF;
                        int y = (pos >> 16) & 0xFFFF;
                        previewTilesEnemyAll.push_back(new Sprite("res/Images/Sprites/Map/PreviewTileRed", x, y, false));
                    }

                    Audio::play(Beep3, 0);
                }
            }
        }

        break;
    }

    case NeutralMenu:
    {
        renderUnits(&unitsEnemy,  6, nullptr);
        renderUnits(&unitsPlayer, 0, nullptr);
        renderMainMenu();

        if (Input::pressedUp())
        {
            Audio::play(Beep5, 0);
            menuIdx--;
            if (menuIdx < 0)
            {
                menuIdx = (int)menuChoices.size() - 1;
            }
        }
        else if (Input::pressedDown())
        {
            Audio::play(Beep5, 0);
            menuIdx = (menuIdx + 1) % menuChoices.size();
        }

        if (Input::pressedA())
        {
            Audio::play(Beep4, 0);
            executeMenuChoice();
        }
        else if (Input::pressedB())
        {
            Audio::play(Beep1, 0);
            resetNeutralHudDescriptions();
            clearPreviewTiles();
            clearPreviewTilesEnemyAll();
            mapState = Neutral;
        }

        break;
    }

    case MovingUnit:
    {
        updateCursor();

        renderPreviewTilesEnemyAll();
        renderPreviewTiles();

        if (Input::pressedB())
        {
            clearPreviewTiles();
            clearPreviewTilesEnemyAll();

            cursorX = selectedUnit->tileX;
            cursorY = selectedUnit->tileY;

            selectedUnit = nullptr;
            resetNeutralHudDescriptions();
            mapState = Neutral;
        }
        else if (Djikstra::unitCanMoveToTile(selectedUnit, cursorX, cursorY))
        {
            renderPreviewArrows();

            if (Input::pressedA())
            {
                //If we're already there, skip the walking animation
                if (cursorX == selectedUnit->tileX &&
                    cursorY == selectedUnit->tileY)
                {
                    clearPreviewTiles();
                    createAttackPreviewTiles(selectedUnit, selectedUnit->getEquipWeaponAttackRange());
                    calculateMenuChoices();
                    mapState = UnitMenu;
                }
                else
                {
                    //We also have to check that there isnt another blue unit on this tile already
                    if (getUnitAtTile(cursorX, cursorY, &unitsPlayer) == nullptr)
                    {
                        clearPreviewTiles();

                        //generate walking path
                        Djikstra::generateWalkingPath(selectedUnit, cursorX, cursorY, &walkingPath);

                        //start walking
                        walkingTimer = (int)walkingPath.size()*8 - 9; //8 frames per tile

                        mapState = WaitingForUnitToMove;
                    }
                }
            }
        }

        renderUnits(&unitsEnemy,  6, nullptr);
        renderUnits(&unitsPlayer, 0, nullptr);

        renderCursor();
        break;
    }

    case WaitingForUnitToMove:
    {
        int id1 = (walkingTimer)/8;
        int id2 = id1 + 1;

        SDL_Point p1 = walkingPath[id1];
        SDL_Point p2 = walkingPath[id2];

        float perc = (walkingTimer % 8)/8.0f;

        float tweenX = p1.x + perc*(p2.x - p1.x);
        float tweenY = p1.y + perc*(p2.y - p1.y);

        int direction = 0;
        if (p2.y > p1.y)
        {
            direction = 2;
        }
        else if (p2.y < p1.y)
        {
            direction = 3;
        }
        else if (p2.x > p1.x)
        {
            direction = 4;
        }
        else if (p2.x < p1.x)
        {
            direction = 5;
        }

        renderUnits(&unitsEnemy,  6, nullptr);

        selectedUnit->x = (int)(tweenX*16);
        selectedUnit->y = (int)(tweenY*16);
        selectedUnit->spriteIndex = direction;

        renderUnits(&unitsPlayer,  0, selectedUnit);
        selectedUnit->render(selectedUnit->x, selectedUnit->y, selectedUnit->spriteIndex, viewportPixelX, viewportPixelY);

        walkingTimer--;
        if (walkingTimer <= 0)
        {
            selectedUnit->tileX = cursorX;
            selectedUnit->tileY = cursorY;
            clearPreviewTiles();
            createAttackPreviewTiles(selectedUnit, selectedUnit->getEquipWeaponAttackRange());
            calculateMenuChoices();
            mapState = UnitMenu;
        }
        break;
    }

    case UnitMenu:
    {
        renderPreviewTiles();
        renderUnits(&unitsEnemy,  6, nullptr);
        renderUnits(&unitsPlayer, 0, nullptr);
        renderMainMenu();

        if (Input::pressedUp())
        {
            menuIdx--;
            if (menuIdx < 0)
            {
                menuIdx = (int)menuChoices.size() - 1;
            }
        }
        else if (Input::pressedDown())
        {
            menuIdx = (menuIdx + 1) % menuChoices.size();
        }

        if (Input::pressedA())
        {
            executeMenuChoice();
        }
        else if (Input::pressedB())
        {
            if (selectedUnitCanGoBack)
            {
                selectedUnit->tileX = selectedUnitOriginalTileX;
                selectedUnit->tileY = selectedUnitOriginalTileY;

                cursorX = selectedUnit->tileX;
                cursorY = selectedUnit->tileY;

                clearPreviewTiles();
                clearPreviewTilesEnemyAll();

                selectedUnit = nullptr;
                resetNeutralHudDescriptions();
                mapState = Neutral;
            }
            else
            {
                //play some sound
            }
        }

        break;
    }

    case UnitMenuItem:
    {
        renderUnits(&unitsEnemy,  6, nullptr);
        renderUnits(&unitsPlayer, 0, nullptr);

        renderItemWindow(selectedUnit, 16, 16, (int)selectedUnit->items.size(), true);
        renderHandCursor(16, 12, itemIdx);
        renderItemWeaponStatsWindow();

        if (Input::pressedUp())
        {
            itemIdx--;
            if (itemIdx < 0)
            {
                itemIdx = (int)selectedUnit->items.size() - 1;
            }
        }
        else if (Input::pressedDown())
        {
            itemIdx = (itemIdx + 1) % selectedUnit->items.size();
        }

        if (Input::pressedA())
        {
            itemEditIdx = 0;
            itemEditChoices.clear();

            if (selectedUnit->canUseWeapon(selectedUnit->items[itemIdx]))
            {
                itemEditChoices.push_back("Equip");
            }

            if (selectedUnit->items[itemIdx].isConsumableByUnit(selectedUnit))
            {
                itemEditChoices.push_back("Use");
            }

            itemEditChoices.push_back("Discard");

            mapState = UnitMenuItemAction;
        }
        else if (Input::pressedB())
        {
            clearPreviewTiles();
            createAttackPreviewTiles(selectedUnit, selectedUnit->getEquipWeaponAttackRange());
            calculateMenuChoices();
            mapState = UnitMenu;
        }

        break;
    }

    case UnitMenuItemAction:
    {
        renderUnits(&unitsEnemy,  6, nullptr);
        renderUnits(&unitsPlayer, 0, nullptr);

        renderItemWindow(selectedUnit, 16, 16, (int)selectedUnit->items.size(), true);
        renderItemWeaponStatsWindow();
        renderItemEditWindow();

        if (Input::pressedUp())
        {
            itemEditIdx--;
            if (itemEditIdx < 0)
            {
                itemEditIdx = (int)itemEditChoices.size() - 1;
            }
        }
        else if (Input::pressedDown())
        {
            itemEditIdx = (itemEditIdx + 1) % itemEditChoices.size();
        }

        if (selectedUnit->items.size() == 0)
        {
            itemEditChoices.clear();
            mapState = UnitMenuItem;
        }

        if (Input::pressedA())
        {
            if (itemEditChoices[itemEditIdx] == "Equip")
            {
                Item itemAtIdx = selectedUnit->items[itemIdx];

                selectedUnit->items.erase(selectedUnit->items.begin() + itemIdx);
                selectedUnit->items.insert(selectedUnit->items.begin(), itemAtIdx);

                itemIdx = 0;
                itemEditIdx = 0;
                itemEditChoices.clear();

                mapState = UnitMenuItem;
            }
            else if (itemEditChoices[itemEditIdx] == "Discard")
            {
                selectedUnit->items.erase(selectedUnit->items.begin() + itemIdx);

                itemIdx = 0;
                itemEditIdx = 0;
                itemEditChoices.clear();

                mapState = UnitMenuItem;

                if (selectedUnit->items.size() == 0)
                {
                    clearPreviewTiles();
                    createAttackPreviewTiles(selectedUnit, selectedUnit->getEquipWeaponAttackRange());
                    calculateMenuChoices();
                    mapState = UnitMenu;
                }
            }
            else if (itemEditChoices[itemEditIdx] == "Use")
            {
                selectedUnit->items[itemIdx].consume(selectedUnit);
                selectedUnit->items[itemIdx].usesRemaining--;

                if (selectedUnit->items[itemIdx].usesRemaining == 0)
                {
                    selectedUnit->items.erase(selectedUnit->items.begin() + itemIdx);
                }

                selectedUnit->isUsed = true;
                selectedUnit = nullptr;

                resetNeutralHudDescriptions();
                clearPreviewTiles();
                clearPreviewTilesEnemyAll();
                mapState = Neutral;
            }
        }
        else if (Input::pressedB())
        {
            mapState = UnitMenuItem;
        }

        break;
    }

    case UnitMenuAttackSelectTarget:
    {
        updateCursor();

        renderPreviewTiles();
        renderUnits(&unitsEnemy,  6, nullptr);
        renderUnits(&unitsPlayer, 0, nullptr);
        renderAttackPreview();

        renderCursor();

        if (Input::pressedA())
        {
            Unit* enemyOnCursor = getUnitAtTile(cursorX, cursorY, &unitsEnemy);
            if (enemyOnCursor != nullptr)
            {
                defendingEnemy = enemyOnCursor;
                attackingTimer = 0;

                Battle::doBattle(selectedUnit, enemyOnCursor);

                resetNeutralHudDescriptions();
                clearPreviewTiles();
                clearPreviewTilesEnemyAll();
                mapState = WaitingForAttackToFinish;
            }
        }
        else if (Input::pressedB())
        {
            clearPreviewTiles();
            createAttackPreviewTiles(selectedUnit, selectedUnit->getEquipWeaponAttackRange());
            calculateMenuChoices();
            mapState = UnitMenu;
        }

        break;
    }

    case WaitingForAttackToFinish:
    {
        renderUnits(&unitsEnemy,  6, defendingEnemy);
        renderUnits(&unitsPlayer, 0, selectedUnit);

        int diffX = defendingEnemy->tileX - selectedUnit->tileX;
        int diffY = defendingEnemy->tileY - selectedUnit->tileY;

        SDL_Point toMove;

        int playerDir = 0;
        int enemyDir  = 0;
        if (diffX > 0)
        {
            playerDir =  5;
            enemyDir  = 10;
            toMove = {1, 0};
        }
        else if (diffX < 0)
        {
            playerDir =  4;
            enemyDir  = 11;
            toMove = {-1, 0};
        }
        else if (diffY > 0)
        {
            playerDir = 3;
            enemyDir  = 8;
            toMove = {0, 1};
        }
        else
        {
            playerDir = 2;
            enemyDir  = 9;
            toMove = {0, -1};
        }

        int turnIdx = attackingTimer/60; //1 second per attack
        int turnTimer = attackingTimer % 60;

        TurnResult turn = Battle::results[turnIdx + 1];

        int turnScale = 1;
        int attackingIndex = playerDir;
        int defendingIndex = 6;
        if (turn.unitDefending == selectedUnit)
        {
            attackingIndex = enemyDir;
            defendingIndex = 0;
            turnScale = -1;
        }

        if (turnTimer >= 22 && turnTimer < 38)
        {
            if (turnTimer < 30)
            {
                turn.unitAttacking->x += toMove.x*turnScale;
                turn.unitAttacking->y += toMove.y*turnScale;
            }
            else
            {
                turn.unitAttacking->x -= toMove.x*turnScale;
                turn.unitAttacking->y -= toMove.y*turnScale;
            }
        }

        SDL_Color colorDefending{255, 255, 255, 255};
        if (turn.hit && turnTimer >= 30 && turnTimer < 45)
        {
            colorDefending.r = (Uint8)((turnTimer - 30)*16);
            colorDefending.g = (Uint8)((turnTimer - 30)*16);
            colorDefending.b = (Uint8)((turnTimer - 30)*16);
        }

        if (turnTimer == 30)
        {
            if (!turn.hit)
            {
                Audio::play(AttackMiss, -1);
            }
            else if (turn.crit)
            {
                Audio::play(AttackCritKill, -1);
            }
            else if (turn.unitLeftHp == 0 || turn.unitRightHp == 0)
            {
                Audio::play(AttackKill, -1);
            }
            else
            {
                Audio::play(AttackHit, -1);
            }
        }

        if (turnTimer == 59 && (turn.unitLeftHp <= 0 || turn.unitRightHp <= 0))
        {
            Audio::play(AttackDieFade, -1);
        }

        turn.unitDefending->render(turn.unitDefending->x, turn.unitDefending->y, defendingIndex, viewportPixelX, viewportPixelY, colorDefending);
        turn.unitAttacking->render(turn.unitAttacking->x, turn.unitAttacking->y, attackingIndex, viewportPixelX, viewportPixelY);

        if (!turn.hit && turnTimer >= 16 && turnTimer < 44)
        {
            attackingMiss->render(turn.unitDefending->x + 8 + viewportPixelX, turn.unitDefending->y + 8 + viewportPixelY, turnTimer - 16);
        }

        attackingHealthDisplay->y = 16;
        if (defendingEnemy->tileY - viewportY < 5)
        {
            attackingHealthDisplay->y = 112;
        }
        attackingHealthDisplay->render( 40, attackingHealthDisplay->y, 0);
        attackingHealthDisplay->render(120, attackingHealthDisplay->y, 1);

        int dispIdx = (int)floor((attackingTimer - 30)/60.0f) + 1;
        TurnResult displayTurn = Battle::results[dispIdx];
        Text::renderText(std::to_string(displayTurn.unitRightHp),   Font::Border, Text::White,  46, attackingHealthDisplay->y + 14, Right,  16);
        Text::renderText(std::to_string(displayTurn.unitLeftHp),    Font::Border, Text::White, 126, attackingHealthDisplay->y + 14, Right,  16);
        Text::renderText(selectedUnit->unitResources.displayName,   Font::Border, Text::White,  49, attackingHealthDisplay->y +  1, Center, 60);
        Text::renderText(defendingEnemy->unitResources.displayName, Font::Border, Text::White, 129, attackingHealthDisplay->y +  1, Center, 60);

        float hpPerc = ((float)displayTurn.unitRightHp)/selectedUnit->maxHp;
        SDL_Rect hpBar;
        hpBar.x = 108;
        hpBar.y = attackingHealthDisplay->y + 21;
        hpBar.w = (int)(-41*(1 - hpPerc));
        hpBar.h = 2;
        SDL_SetRenderDrawColor(Global::sdlRenderer, 115, 49, 0, 255);
        SDL_RenderFillRect(Global::sdlRenderer, &hpBar);

        hpPerc = ((float)displayTurn.unitLeftHp)/defendingEnemy->maxHp;
        hpBar.x = 188;
        hpBar.w = (int)(-41*(1 - hpPerc));
        SDL_RenderFillRect(Global::sdlRenderer, &hpBar);
        SDL_SetRenderDrawColor(Global::sdlRenderer, 255, 255, 255, 255);


        attackingTimer++;
        if (attackingTimer >= (Battle::results.size() - 1)*60)
        {
            attackingTimer = 0;

            selectedUnit->isUsed = true;

            if (selectedUnit->hp <= 0)
            {
                unitsPlayer.erase(unitsPlayer.begin() + Util::getIndex(&unitsPlayer, selectedUnit));
                delete selectedUnit;
                selectedUnit = nullptr;
            }

            if (defendingEnemy->hp <= 0)
            {
                unitsEnemy.erase(unitsEnemy.begin() + Util::getIndex(&unitsEnemy, defendingEnemy));
                delete defendingEnemy;
                defendingEnemy = nullptr;
            }

            resetNeutralHudDescriptions();
            clearPreviewTiles();
            clearPreviewTilesEnemyAll();
            mapState = Neutral;
        }
        break;
    }

    case UnitMenuTradeSelectTarget:
    {
        updateCursor();
        renderUnits(&unitsEnemy,  6, nullptr);
        renderUnits(&unitsPlayer, 0, nullptr);
        renderCursor();

        if (Input::pressedA())
        {
            Unit* unitOnCursor = getUnitAtTile(cursorX, cursorY, &unitsPlayer);
            if (unitOnCursor != nullptr)
            {
                int xDiff = cursorX - selectedUnit->tileX;
                int yDiff = cursorY - selectedUnit->tileY;

                if (abs(xDiff) + abs(yDiff) == 1)
                {
                    tradingUnit = unitOnCursor;
                    mapState = UnitMenuTrading;
                }
            }
        }
        else if (Input::pressedB())
        {
            clearPreviewTiles();
            createAttackPreviewTiles(selectedUnit, selectedUnit->getEquipWeaponAttackRange());
            calculateMenuChoices();
            mapState = UnitMenu;
        }

        break;
    }

    case UnitMenuTrading:
    {
        selectedUnit->sprMugshot->x = 16;
        selectedUnit->sprMugshot->y = 0;
        selectedUnit->sprMugshot->scaleX = -1;
        selectedUnit->sprMugshot->render();

        tradingUnit->sprMugshot->x = 16 + 112;
        tradingUnit->sprMugshot->y = 0;
        tradingUnit->sprMugshot->scaleX = 1;
        tradingUnit->sprMugshot->render();

        renderItemWindow(selectedUnit, 12,   16*4 + 8, 5, true);
        renderItemWindow(tradingUnit,  16*7 + 12, 16*4 + 8, 5, true);

        if (tradeSideLeft || tradeIsLocked)
        {
            renderHandCursor(12, 16*4 + 5, tradeLeftIdx);
        }

        if (!tradeSideLeft || tradeIsLocked)
        {
            renderHandCursor(16*7 + 12, 16*4 + 5, tradeRightIdx);
        }

        if (Input::pressedA())
        {
            if (tradeIsLocked)
            {
                Item itemLeft(None);
                Item itemRight(None);
                if (tradeLeftIdx < selectedUnit->items.size())
                {
                    itemLeft = selectedUnit->items[tradeLeftIdx];
                }
                if (tradeRightIdx < tradingUnit->items.size())
                {
                    itemRight = tradingUnit->items[tradeRightIdx];
                }

                if (!(itemLeft.id == None && itemRight.id == None))
                {
                    if (itemLeft.id == None)
                    {
                        selectedUnit->items.push_back(itemRight);
                    }
                    else if (itemRight.id == None)
                    {
                        selectedUnit->items.erase(selectedUnit->items.begin() + tradeLeftIdx);
                    }
                    else
                    {
                        selectedUnit->items[tradeLeftIdx] = itemRight;
                    }

                    if (itemRight.id == None)
                    {
                        tradingUnit->items.push_back(itemLeft);
                    }
                    else if (itemLeft.id == None)
                    {
                        tradingUnit->items.erase(tradingUnit->items.begin() + tradeRightIdx);
                    }
                    else
                    {
                        tradingUnit->items[tradeRightIdx] = itemLeft;
                    }

                    tradeIsLocked = false;
                    tradeRightIdx = 0;
                    tradeLeftIdx = 0;
                }
            }
            else
            {
                tradeIsLocked = true;
                if (tradeSideLeft)
                {
                    tradeRightIdx = 0;
                }
                else
                {
                    tradeLeftIdx = 0;
                }
            }
        }
        else if (Input::pressedB())
        {
            if (tradeIsLocked)
            {
                tradeIsLocked = false;
            }
            else
            {
                clearPreviewTiles();
                createAttackPreviewTiles(selectedUnit, selectedUnit->getEquipWeaponAttackRange());
                calculateMenuChoices();
                mapState = UnitMenu;
            }
        }
        else if (Input::pressedUp())
        {
            int* idx = &tradeLeftIdx;

            if ((!tradeSideLeft && !tradeIsLocked) ||
                ( tradeSideLeft &&  tradeIsLocked))
            {
                idx = &tradeRightIdx;
            }

            if ((*idx) > 0)
            {
                (*idx) = (*idx) - 1;
            }
        }
        else if (Input::pressedDown())
        {
            std::vector<Item>* items = &selectedUnit->items;
            int* idx = &tradeLeftIdx;

            if ((!tradeSideLeft && !tradeIsLocked) ||
                ( tradeSideLeft &&  tradeIsLocked))
            {
                items = &tradingUnit->items;
                idx = &tradeRightIdx;
            }

            if ((*idx) < 4 && (*idx) < items->size())
            {
                (*idx) = (*idx) + 1;
            }
        }
        else if (Input::pressedLeft())
        {
            if (!tradeIsLocked && !tradeSideLeft)
            {
                tradeSideLeft = true;
                tradeLeftIdx = 0;
            }
        }
        else if (Input::pressedRight())
        {
            if (!tradeIsLocked && tradeSideLeft)
            {
                tradeSideLeft = false;
                tradeRightIdx = 0;
            }
        }

        break;
    }

    case UnitMenuStaffSelect:
    {
        renderUnits(&unitsEnemy,  6, nullptr);
        renderUnits(&unitsPlayer, 0, nullptr);

        renderStaffWindow();
        renderStaffDescriptionWindow();
        renderHandCursor(16, 8, staffIdx);

        if (Input::pressedUp())
        {
            staffIdx--;
            if (staffIdx < 0)
            {
                staffIdx = (int)staffChoices.size() - 1;
            }
        }
        else if (Input::pressedDown())
        {
            staffIdx = (staffIdx + 1) % staffChoices.size();
        }

        if (Input::pressedA())
        {
            Item* staff = staffChoices[staffIdx];
            std::unordered_set<int> ranges = staff->getStaffRange();
            clearPreviewTiles();
            createStaffPreviewTiles(selectedUnit, ranges);

            //todo: search through units to find one in the green tiles
            std::vector<Unit*> units = getAdjacentUnits(selectedUnit, &unitsPlayer);
            for (int i = 0; i < units.size(); i++)
            {
                if (units[i]->hp < units[i]->maxHp)
                {
                    cursorX = units[i]->tileX;
                    cursorY = units[i]->tileY;
                    break;
                }
            }

            mapState = UnitMenuStaffSelectTarget;
        }
        else if (Input::pressedB())
        {
            clearPreviewTiles();
            createAttackPreviewTiles(selectedUnit, selectedUnit->getEquipWeaponAttackRange());
            calculateMenuChoices();
            mapState = UnitMenu;
        }
        break;
    }

    case UnitMenuStaffSelectTarget:
    {
        updateCursor();

        renderPreviewTiles();
        renderUnits(&unitsEnemy,  6, nullptr);
        renderUnits(&unitsPlayer, 0, nullptr);

        renderCursor();

        if (Input::pressedA())
        {
            Unit* friendAtCursor = getUnitAtTile(cursorX, cursorY, &unitsPlayer);
            if (friendAtCursor != nullptr)
            {
                //todo: check if friendAtCursor is in a green tile
                Item* staff = staffChoices[staffIdx];
                switch (staff->id)
                {
                    case Heal:
                    case Physic:
                        friendAtCursor->hp = Util::min(friendAtCursor->hp += 10 + selectedUnit->mag, friendAtCursor->maxHp);
                        break;

                    case Mend:
                        friendAtCursor->hp = Util::min(friendAtCursor->hp += 20 + selectedUnit->mag, friendAtCursor->maxHp);
                        break;

                    case Recover:
                        friendAtCursor->hp = friendAtCursor->maxHp;
                        break;

                    case Barrier:
                        friendAtCursor->res += 7;
                        break;

                    default:
                        break;
                }
                staff->usesRemaining--;
                selectedUnit->isUsed = true;
                clearPreviewTiles();
                mapState = WaitingForStaffToFinish;
            }
        }
        else if (Input::pressedB())
        {
            clearPreviewTiles();
            mapState = UnitMenuStaffSelect;
        }

        break;
    }

    case WaitingForStaffToFinish:
    {
        resetNeutralHudDescriptions();
        clearPreviewTiles();
        clearPreviewTilesEnemyAll();
        mapState = Neutral;
        break;
    }

    case PlayerPhaseEnding:
    {
        turnChangeTimer--;

        resetNeutralHudDescriptions();
        renderUnits(&unitsEnemy,  6, nullptr);
        renderUnits(&unitsPlayer, 0, nullptr);

        if (turnChangeTimer == TURN_CHANGE_TIMER_MAX - 15)
        {
            Audio::play(TurnChange, -1);
        }

        if (turnChangeTimer <= 0)
        {
            isPhaseEnemy  = true;
            isPhasePlayer = false;

            for (int i = 0; i < unitsPlayer.size(); i++)
            {
                unitsPlayer[i]->isUsed = false;
            }

            mapState = EnemyPhaseCalculating;
            enemyIdx = (int)unitsEnemy.size()-1;
            turnChangeTimer = TURN_CHANGE_TIMER_MAX;
        }
        else
        {
            renderTurnChange(1);
        }
        break;
    }

    default:
    {
        break;
    }
    }

    updateCamera();
}

void Map::enemyPhase()
{
    switch (mapState)
    {
    case EnemyPhaseCalculating:
    {
        renderUnits(&unitsPlayer, 0, nullptr);
        renderUnits(&unitsEnemy, 6, nullptr);

        if (enemyIdx == -1)
        {
            mapState = EnemyPhaseEnding;
            turnChangeTimer = TURN_CHANGE_TIMER_MAX;
            for (Unit* enemy : unitsEnemy)
            {
                enemy->isUsed = false;
            }
            break;
        }

        Unit* enemy = unitsEnemy[enemyIdx];

        // Need to make a copy of inventory because we move thigns around during calculations.
        std::vector<Item> originalEnemyInventory;
        for (Item item : enemy->items)
        {
            originalEnemyInventory.push_back(item);
        }

        long long maxScore = 0;

        std::vector<SDL_Point> walkableTiles;
        Djikstra::calculateBlueTiles(enemy, &walkableTiles, &unitsEnemy, &unitsPlayer);
        for (int i = 0; i < (int)walkableTiles.size(); i++)
        {
            SDL_Point tile = walkableTiles[i];
            for (int j = 0; j < (int)originalEnemyInventory.size(); j++)
            {
                // Put the weapon at the startt of the vector so that it is considered the "equipped" weapon during calculations.
                enemy->items.clear();
                enemy->items.push_back(originalEnemyInventory[j]);
                Item item = originalEnemyInventory.at(j);

                if (item.isWeapon() && enemy->canUseWeapon(item))
                {
                    std::unordered_set<int> ranges = item.getWeaponRange();
                    std::vector<SDL_Point> redTiles = calculatePreviewTilesAtTile(tile.x, tile.y, ranges);
                    std::vector<Unit*> unitsICanAttack = getUnitsInTiles(&unitsPlayer, &redTiles);
                    
                    for (Unit* attackableUnit : unitsICanAttack)
                    {
                        // Place the unit on the tile he will attack from to calculate stats.
                        int enemyOriginalTileX = enemy->tileX;
                        int enemyOriginalTileY = enemy->tileY;
                        enemy->tileX = tile.x;
                        enemy->tileY = tile.y;

                        int myAttack     = 0;
                        int myHit        = 0;
                        int myCrit       = 0;
                        int playerAttack = 0;
                        int playerHit    = 0;
                        int playerCrit   = 0;
                        enemy->calculateCombatStatsVsUnit(attackableUnit, &myAttack, &myHit, &myCrit);
                        attackableUnit->calculateCombatStatsVsUnit(enemy, &playerAttack, &playerHit, &playerCrit);

                        // Put the enemy back in their original tile.
                        enemy->tileX = enemyOriginalTileX;
                        enemy->tileY = enemyOriginalTileY;

                        long long scoreOffense = myAttack*myHit*(myCrit+100LL)*(150LL - playerAttack)*(200LL - playerHit)*(200LL - playerCrit);

                        if (scoreOffense > maxScore)
                        {
                            maxScore = scoreOffense;
                            enemyTileToAttackFrom = tile;
                            unitEnemyWillAttack = attackableUnit;
                            weaponEnemyAttacksWith = item;
                        }
                    }
                }
            }
        }

        if (maxScore > 0) // we found someone we can attack, start walking towards them.
        {
            // First we need to order the items so that the attacking weapon is at the start of the inventory.
            enemy->items.clear();
            enemy->items.push_back(weaponEnemyAttacksWith);
            bool alreadyFoundUsedWeapon = false;
            for (Item item : originalEnemyInventory)
            {
                if (item == weaponEnemyAttacksWith)
                {
                    if (!alreadyFoundUsedWeapon)
                    {
                        alreadyFoundUsedWeapon = true; //this is the weapon we already added, so dont add it again.   
                    }
                    else
                    {
                        enemy->items.push_back(item); //we had 2 or more identical copies of the weapon, so add them.
                    }
                }
                else
                {
                    enemy->items.push_back(item);
                }
            }


            Djikstra::generateWalkingPath(enemy, enemyTileToAttackFrom.x, enemyTileToAttackFrom.y, &enemyWalkingPath);

            //start walking
            enemyWalkingTimer = (int)enemyWalkingPath.size()*8 - 9; //8 frames per tile

            mapState = EnemyPhaseWaitingForUnitToMove;
            enemyStateAfterCurrentState = EnemyPhaseWaitingForAttackToFinish;
        }
        else //there arent anyone we can do damage to, do nothing.
        {
            // Restore the original inventory
            enemy->items.clear();
            for (Item item : originalEnemyInventory)
            {
                enemy->items.push_back(item);
            }

            //if (objective == MapObjective::Protect) //Head for the sieze tile
            {
                int dx = std::abs(seizeTile.x - enemy->tileX);
                int dy = std::abs(seizeTile.y - enemy->tileY);
                //if (dx < 12 && dy < 12 && dx != 0 && dy != 0)
                if (dx != 0 && dy != 0)
                {
                    Djikstra::generateWalkingPath(enemy, seizeTile.x, seizeTile.y, &enemyWalkingPath);
                    //start walking
                    enemyWalkingTimer = (int)enemyWalkingPath.size()*8 - 9; //8 frames per tile

                    mapState = EnemyPhaseWaitingForUnitToMove;
                    enemyStateAfterCurrentState = EnemyPhaseCalculating;
                }
                else
                {
                    enemyIdx--;
                }
            }

            //enemyIdx--;
        }

        break;
    }

    case EnemyPhaseWaitingForUnitToMove:
    {
        Unit* movingUnit = unitsEnemy[enemyIdx];

        int id1 = (enemyWalkingTimer)/8;
        int id2 = id1 + 1;

        SDL_Point p1 = enemyWalkingPath[id1];
        SDL_Point p2 = enemyWalkingPath[id2];

        float perc = (enemyWalkingTimer % 8)/8.0f;

        float tweenX = p1.x + perc*(p2.x - p1.x);
        float tweenY = p1.y + perc*(p2.y - p1.y);

        int direction = 0;
        if (p2.y > p1.y)
        {
            direction = 2;
        }
        else if (p2.y < p1.y)
        {
            direction = 3;
        }
        else if (p2.x > p1.x)
        {
            direction = 4;
        }
        else if (p2.x < p1.x)
        {
            direction = 5;
        }

        renderUnits(&unitsPlayer, 0, nullptr);

        movingUnit->x = (int)(tweenX*16);
        movingUnit->y = (int)(tweenY*16);
        movingUnit->spriteIndex = direction+6;

        renderUnits(&unitsEnemy, 6, movingUnit);
        movingUnit->render(movingUnit->x, movingUnit->y, movingUnit->spriteIndex, viewportPixelX, viewportPixelY);

        enemyWalkingTimer--;
        if (enemyWalkingTimer <= 0)
        {
            enemyAttackingTimer = 0;

            if (enemyStateAfterCurrentState == EnemyPhaseWaitingForAttackToFinish)
            {
                movingUnit->tileX = enemyTileToAttackFrom.x;
                movingUnit->tileY = enemyTileToAttackFrom.y;

                Battle::doBattle(movingUnit, unitEnemyWillAttack);

                resetNeutralHudDescriptions();
                mapState = EnemyPhaseWaitingForAttackToFinish;
            }
            else if (enemyStateAfterCurrentState == EnemyPhaseCalculating)
            {
                movingUnit->tileX = p1.x;
                movingUnit->tileY = p1.y;

                mapState = EnemyPhaseCalculating;
                enemyIdx--;
            }
        }

        cursorX = p1.x;
        cursorY = p1.y;
        updateCamera();

        break;
    }

    case EnemyPhaseWaitingForAttackToFinish:
    {
        Unit* attackingUnit = unitsEnemy[enemyIdx];
        Unit* defendingPlayer = unitEnemyWillAttack;

        cursorX = defendingPlayer->tileX;
        cursorY = defendingPlayer->tileY;
        updateCamera();

        renderUnits(&unitsPlayer, 0, defendingPlayer);
        renderUnits(&unitsEnemy,  6, attackingUnit);

        int diffX = defendingPlayer->tileX - attackingUnit->tileX;
        int diffY = defendingPlayer->tileY - attackingUnit->tileY;

        SDL_Point toMove;

        int playerDir = 0;
        int enemyDir  = 0;
        if (diffX > 0)
        {
            playerDir =  4;
            enemyDir  = 11;
            toMove = {1, 0};
        }
        else if (diffX < 0)
        {
            playerDir =  5;
            enemyDir  = 10;
            toMove = {-1, 0};
        }
        else if (diffY > 0)
        {
            playerDir = 2;
            enemyDir  = 9;
            toMove = {0, 1};
        }
        else
        {
            playerDir = 3;
            enemyDir  = 8;
            toMove = {0, -1};
        }

        int turnIdx = enemyAttackingTimer/60; //1 second per attack
        int turnTimer = enemyAttackingTimer % 60;

        TurnResult turn = Battle::results[turnIdx + 1];

        int turnScale = 1;
        int attackingIndex = enemyDir;
        int defendingIndex = 0;
        if (turn.unitDefending == attackingUnit)
        {
            attackingIndex = playerDir;
            defendingIndex = 6;
            turnScale = -1;
        }

        if (turnTimer >= 22 && turnTimer < 38)
        {
            if (turnTimer < 30)
            {
                turn.unitAttacking->x += toMove.x*turnScale;
                turn.unitAttacking->y += toMove.y*turnScale;
            }
            else
            {
                turn.unitAttacking->x -= toMove.x*turnScale;
                turn.unitAttacking->y -= toMove.y*turnScale;
            }
        }

        SDL_Color colorDefending{255, 255, 255, 255};
        if (turn.hit && turnTimer >= 30 && turnTimer < 45)
        {
            colorDefending.r = (Uint8)((turnTimer - 30)*16);
            colorDefending.g = (Uint8)((turnTimer - 30)*16);
            colorDefending.b = (Uint8)((turnTimer - 30)*16);
        }

        if (turnTimer == 30)
        {
            if (!turn.hit)
            {
                Audio::play(AttackMiss, -1);
            }
            else if (turn.crit)
            {
                Audio::play(AttackCritKill, -1);
            }
            else if (turn.unitLeftHp == 0 || turn.unitRightHp == 0)
            {
                Audio::play(AttackKill, -1);
            }
            else
            {
                Audio::play(AttackHit, -1);
            }
        }

        if (turnTimer == 59 && (turn.unitLeftHp <= 0 || turn.unitRightHp <= 0))
        {
            Audio::play(AttackDieFade, -1);
        }

        turn.unitDefending->render(turn.unitDefending->x, turn.unitDefending->y, defendingIndex, viewportPixelX, viewportPixelY, colorDefending);
        turn.unitAttacking->render(turn.unitAttacking->x, turn.unitAttacking->y, attackingIndex, viewportPixelX, viewportPixelY);

        if (!turn.hit && turnTimer >= 16 && turnTimer < 44)
        {
            attackingMiss->render(turn.unitDefending->x + 8 + viewportPixelX, turn.unitDefending->y + 8 + viewportPixelY, turnTimer - 16);
        }

        attackingHealthDisplay->y = 16;
        if (defendingPlayer->tileY - viewportY < 5)
        {
            attackingHealthDisplay->y = 112;
        }
        attackingHealthDisplay->render( 40, attackingHealthDisplay->y, 1);
        attackingHealthDisplay->render(120, attackingHealthDisplay->y, 0);

        int dispIdx = (int)floor((enemyAttackingTimer - 30)/60.0f) + 1;
        TurnResult displayTurn = Battle::results[dispIdx];
        Text::renderText(std::to_string(displayTurn.unitRightHp),    Font::Border, Text::White,  46, attackingHealthDisplay->y + 14, Right,  16);
        Text::renderText(std::to_string(displayTurn.unitLeftHp),     Font::Border, Text::White, 126, attackingHealthDisplay->y + 14, Right,  16);
        Text::renderText(attackingUnit->unitResources.displayName,   Font::Border, Text::White,  49, attackingHealthDisplay->y +  1, Center, 60);
        Text::renderText(defendingPlayer->unitResources.displayName, Font::Border, Text::White, 129, attackingHealthDisplay->y +  1, Center, 60);

        float hpPerc = ((float)displayTurn.unitRightHp)/attackingUnit->maxHp;
        SDL_Rect hpBar;
        hpBar.x = 108;
        hpBar.y = attackingHealthDisplay->y + 21;
        hpBar.w = (int)(-41*(1 - hpPerc));
        hpBar.h = 2;
        SDL_SetRenderDrawColor(Global::sdlRenderer, 115, 49, 0, 255);
        SDL_RenderFillRect(Global::sdlRenderer, &hpBar);

        hpPerc = ((float)displayTurn.unitLeftHp)/defendingPlayer->maxHp;
        hpBar.x = 188;
        hpBar.w = (int)(-41*(1 - hpPerc));
        SDL_RenderFillRect(Global::sdlRenderer, &hpBar);
        SDL_SetRenderDrawColor(Global::sdlRenderer, 255, 255, 255, 255);


        enemyAttackingTimer++;
        if (enemyAttackingTimer >= (Battle::results.size() - 1)*60)
        {
            enemyAttackingTimer = 0;

            attackingUnit->isUsed = true;

            if (attackingUnit->hp <= 0)
            {
                unitsEnemy.erase(unitsEnemy.begin() + Util::getIndex(&unitsEnemy, attackingUnit));
                delete attackingUnit;
                attackingUnit = nullptr;
            }

            if (defendingPlayer->hp <= 0)
            {
                unitsPlayer.erase(unitsPlayer.begin() + Util::getIndex(&unitsPlayer, defendingPlayer));
                delete defendingPlayer;
                defendingPlayer = nullptr;
            }

            resetNeutralHudDescriptions();
            mapState = EnemyPhaseCalculating;
            enemyIdx--;
        }
        break;
    }

    case EnemyPhaseEnding:
    {
        turnChangeTimer--;

        renderUnits(&unitsPlayer, 0, nullptr);
        renderUnits(&unitsEnemy,  6, nullptr);

        if (turnChangeTimer == TURN_CHANGE_TIMER_MAX - 15)
        {
            Audio::play(TurnChange, -1);
        }

        if (turnChangeTimer == 0)
        {
            isPhaseEnemy  = false;
            isPhasePlayer = true;
            clearPreviewTiles();
            clearPreviewTilesEnemyAll();
            mapState = Neutral;
        }
        else
        {
            renderTurnChange(0);
        }
        break;
    }

    default: break;
    }
}

void Map::updateCursor()
{
    int prevX = cursorX;
    int prevY = cursorY;

    if (Input::pressedUp() && cursorY > 0)
    {
        cursorY--;
    }
    if (Input::pressedDown() && cursorY < tilesHeight - 1)
    {
        cursorY++;
    }
    if (Input::pressedLeft() && cursorX > 0)
    {
        cursorX--;
    }
    if (Input::pressedRight() && cursorX < tilesWidth - 1)
    {
        cursorX++;
    }

    if (cursorX != prevX ||
        cursorY != prevY)
    {
        Audio::play(Cursor, 3);
    }
}

void Map::updateCamera()
{
    if (cursorX <= viewportX + 2)
    {
        viewportX--;
    }
    if (cursorY <= viewportY + 2)
    {
        viewportY--;
    }
    if (cursorX >= viewportX + 13)
    {
        viewportX++;
    }
    if (cursorY >= viewportY + 8)
    {
        viewportY++;
    }

    viewportX = Util::clamp(0, viewportX, tilesWidth  - 15);
    viewportY = Util::clamp(0, viewportY, tilesHeight - 10);

    int viewportPixelTargetX = -viewportX*16;
    int viewportPixelTargetY = -viewportY*16;

    const float CAMERA_SMOOTH_FACTOR = 0.25f;
    viewportPixelX = Util::approach(viewportPixelX, viewportPixelTargetX, CAMERA_SMOOTH_FACTOR);
    viewportPixelY = Util::approach(viewportPixelY, viewportPixelTargetY, CAMERA_SMOOTH_FACTOR);
}

void Map::clearPreviewTiles()
{
    for (int i = 0; i < previewTilesBlue.size(); i++)
    {
        delete previewTilesBlue[i];
    }
    previewTilesBlue.clear();

    for (int i = 0; i < previewTilesRed.size(); i++)
    {
        delete previewTilesRed[i];
    }
    previewTilesRed.clear();

    for (int i = 0; i < previewTilesGreen.size(); i++)
    {
        delete previewTilesGreen[i];
    }
    previewTilesGreen.clear();
}

void Map::clearPreviewTilesEnemyAll()
{
    for (int i = 0; i < previewTilesEnemyAll.size(); i++)
    {
        delete previewTilesEnemyAll[i];
    }
    previewTilesEnemyAll.clear();
}

void Map::renderPreviewTiles()
{
    for (int i = 0; i < previewTilesBlue.size(); i++)
    {
        int tileX = previewTilesBlue[i]->x;
        int tileY = previewTilesBlue[i]->y;
        int pixelX = viewportPixelX + tileX*16;
        int pixelY = viewportPixelY + tileY*16;
        previewTilesBlue[i]->x = pixelX;
        previewTilesBlue[i]->y = pixelY;
        previewTilesBlue[i]->imageIndex++;
        previewTilesBlue[i]->render();
        previewTilesBlue[i]->x = tileX;
        previewTilesBlue[i]->y = tileY;
    }

    for (int i = 0; i < previewTilesRed.size(); i++)
    {
        int tileX = previewTilesRed[i]->x;
        int tileY = previewTilesRed[i]->y;
        int pixelX = viewportPixelX + tileX*16;
        int pixelY = viewportPixelY + tileY*16;
        previewTilesRed[i]->x = pixelX;
        previewTilesRed[i]->y = pixelY;
        previewTilesRed[i]->imageIndex++;
        previewTilesRed[i]->render();
        previewTilesRed[i]->x = tileX;
        previewTilesRed[i]->y = tileY;
    }

    for (int i = 0; i < previewTilesGreen.size(); i++)
    {
        int tileX = previewTilesGreen[i]->x;
        int tileY = previewTilesGreen[i]->y;
        int pixelX = viewportPixelX + tileX*16;
        int pixelY = viewportPixelY + tileY*16;
        previewTilesGreen[i]->x = pixelX;
        previewTilesGreen[i]->y = pixelY;
        previewTilesGreen[i]->imageIndex++;
        previewTilesGreen[i]->render();
        previewTilesGreen[i]->x = tileX;
        previewTilesGreen[i]->y = tileY;
    }
}

void Map::renderPreviewTilesEnemyAll()
{
    for (int i = 0; i < previewTilesEnemyAll.size(); i++)
    {
        int tileX = previewTilesEnemyAll[i]->x;
        int tileY = previewTilesEnemyAll[i]->y;
        int pixelX = viewportPixelX + tileX*16;
        int pixelY = viewportPixelY + tileY*16;
        previewTilesEnemyAll[i]->x = pixelX;
        previewTilesEnemyAll[i]->y = pixelY;
        previewTilesEnemyAll[i]->imageIndex++;
        previewTilesEnemyAll[i]->render();
        previewTilesEnemyAll[i]->x = tileX;
        previewTilesEnemyAll[i]->y = tileY;
    }
}

Unit* Map::getUnitAtTile(int tileX, int tileY, std::vector<Unit*>* units)
{
    for (int i = 0; i < units->size(); i++)
    {
        if (units->at(i)->tileX == tileX && units->at(i)->tileY == tileY)
        {
            return units->at(i);
        }
    }

    return nullptr;
}

std::vector<Unit*> Map::getAdjacentUnits(Unit* unit, std::vector<Unit*>* units)
{
    std::vector<Unit*> adjacentUnits;

    Unit* unit1 = getUnitAtTile(unit->tileX - 1, unit->tileY,     units);
    Unit* unit2 = getUnitAtTile(unit->tileX,     unit->tileY - 1, units);
    Unit* unit3 = getUnitAtTile(unit->tileX + 1, unit->tileY,     units);
    Unit* unit4 = getUnitAtTile(unit->tileX,     unit->tileY + 1, units);

    if (unit1 != nullptr) { adjacentUnits.push_back(unit1); }
    if (unit2 != nullptr) { adjacentUnits.push_back(unit2); }
    if (unit3 != nullptr) { adjacentUnits.push_back(unit3); }
    if (unit4 != nullptr) { adjacentUnits.push_back(unit4); }

    return adjacentUnits;
}

std::vector<Unit*> Map::getEnemiesInRedTiles()
{
    std::vector<Unit*> enemies;

    for (int i = 0; i < previewTilesRed.size(); i++)
    {
        int tileX = previewTilesRed[i]->x;
        int tileY = previewTilesRed[i]->y;

        for (int e = 0; e < unitsEnemy.size(); e++)
        {
            if (unitsEnemy[e]->tileX == tileX &&
                unitsEnemy[e]->tileY == tileY)
            {
                enemies.push_back(unitsEnemy[e]);
            }
        }
    }

    return enemies;
}

std::vector<Unit*> Map::getUnitsInTiles(std::vector<Unit*>* units, std::vector<SDL_Point>* tilesToSearch)
{
    std::vector<Unit*> unitsInTiles;

    for (auto itr = units->begin(); itr != units->end(); ++itr)
    {
        Unit* unit = *itr;

        for (auto itr2 = tilesToSearch->begin(); itr2 != tilesToSearch->end(); ++itr2)
        {
            SDL_Point tile = *itr2;

            if (unit->tileX == tile.x &&
                unit->tileY == tile.y)
            {
                unitsInTiles.push_back(unit);
                break;
            }
        }
    }

    return unitsInTiles;
}

MapTile Map::getTile(int x, int y, std::vector<Unit*>* unpassableUnits)
{
    if (x < 0 || y < 0 || x >= tilesWidth || y >= tilesHeight || (getUnitAtTile(x, y, unpassableUnits) != nullptr))
    {
        return MapTile(MapTileType::NoPass, "OutOfBounds");
    }

    return tiles[x + tilesWidth*y];
}

void Map::renderPreviewArrows()
{
    Djikstra::generateWalkingPath(selectedUnit, cursorX, cursorY, &walkingPath);

    if (walkingPath.size() >= 2)
    {
        //last tile = special case
        SDL_Point p2 = walkingPath[0];
        SDL_Point p1 = walkingPath[1];
        int dirIdx = 0;
        if (p2.y < p1.y) //up
        {
            dirIdx = 4;
        }
        else if (p2.y > p1.y) //down
        {
            dirIdx = 5;
        }
        else if (p2.x < p1.x) //left
        {
            dirIdx = 6;
        }
        else if (p2.x > p1.x) //right
        {
            dirIdx = 7;
        }
        previewArrowSprite->imageIndex = dirIdx;
        previewArrowSprite->x = p2.x*16 + viewportPixelX;
        previewArrowSprite->y = p2.y*16 + viewportPixelY;
        previewArrowSprite->render();

        for (int i = 1; i < walkingPath.size() - 1; i++)
        {
            SDL_Point p3 = walkingPath[i - 1];
            p2           = walkingPath[i + 0];
            p1           = walkingPath[i + 1];

            SDL_Point diffPrev = {p2.x - p1.x, p2.y - p1.y};
            SDL_Point diffNext = {p3.x - p2.x, p3.y - p2.y};

            dirIdx = 4;
            if (diffPrev.x == 0 && diffNext.x == 0)
            {
                dirIdx = 0;
            }
            else if (diffPrev.y == 0 && diffNext.y == 0)
            {
                dirIdx = 2;
            }
            else if (diffPrev.x == 1 && diffNext.y == -1)
            {
                dirIdx = 11;
            }
            else if (diffPrev.y == 1 && diffNext.x == -1)
            {
                dirIdx = 11;
            }
            else if (diffPrev.x == -1 && diffNext.y == -1)
            {
                dirIdx = 10;
            }
            else if (diffPrev.y == 1 && diffNext.x == 1)
            {
                dirIdx = 10;
            }
            else if (diffPrev.x == 1 && diffNext.y == 1)
            {
                dirIdx = 9;
            }
            else if (diffPrev.y == -1 && diffNext.x == -1)
            {
                dirIdx = 9;
            }
            else if (diffPrev.y == -1 && diffNext.x == 1)
            {
                dirIdx = 8;
            }
            else if (diffPrev.x == -1 && diffNext.y == 1)
            {
                dirIdx = 8;
            }

            previewArrowSprite->imageIndex = dirIdx;
            previewArrowSprite->x = p2.x*16 + viewportPixelX;
            previewArrowSprite->y = p2.y*16 + viewportPixelY;
            previewArrowSprite->render();
        }
    }
}

void Map::calculateMenuChoices()
{
    menuIdx = 0;
    menuChoices.clear();

    std::vector<Unit*> enemiesWeCanAttack = getEnemiesInRedTiles();
    if (enemiesWeCanAttack.size() > 0)
    {
        menuChoices.push_back("Attack");
    }

    staffIdx = 0;
    staffChoices.clear();
    for (int i = 0; i < selectedUnit->items.size(); i++)
    {
        if (unitCanUseStaff(selectedUnit, selectedUnit->items[i], &unitsPlayer, &unitsEnemy))
        {
            staffChoices.push_back(&selectedUnit->items[i]);
        }
    }
    if (staffChoices.size() > 0)
    {
        menuChoices.push_back("Staff");
    }

    if (selectedUnit->items.size() > 0)
    {
        menuChoices.push_back("Item");
    }

    std::vector<Unit*> adjacentPlayerUnits = getAdjacentUnits(selectedUnit, &unitsPlayer);

    if (adjacentPlayerUnits.size() != 0)
    {
        menuChoices.push_back("Trade");
    }

    bool addedRescue = false;
    bool addedTake = false;
    if (selectedUnit->rescuedUnit == nullptr)
    {
        for (int i = 0; i < adjacentPlayerUnits.size(); i++)
        {
            if (selectedUnit->aid >= adjacentPlayerUnits[i]->con &&
                adjacentPlayerUnits[i]->rescuedUnit == nullptr)
            {
                if (!addedRescue)
                {
                    menuChoices.push_back("Rescue");
                    addedRescue = true;
                }
            }
            else if (adjacentPlayerUnits[i]->rescuedUnit != nullptr &&
                     selectedUnit->aid >= adjacentPlayerUnits[i]->rescuedUnit->con)
            {
                if (!addedTake)
                {
                    menuChoices.push_back("Take");
                    addedTake = true;
                }
            }
        }
    }
    else
    {
        //Any others we can give the unit to?
        for (int i = 0; i < adjacentPlayerUnits.size(); i++)
        {
            if (adjacentPlayerUnits[i]->rescuedUnit == nullptr &&
                adjacentPlayerUnits[i]->aid >= selectedUnit->rescuedUnit->con)
            {
                menuChoices.push_back("Give");
                break;
            }
        }

        //Is the tile a wall/void/Enemy?
        MapTile tile1 = getTile(selectedUnit->tileX - 1, selectedUnit->tileY    , &unitsEnemy);
        MapTile tile2 = getTile(selectedUnit->tileX    , selectedUnit->tileY - 1, &unitsEnemy);
        MapTile tile3 = getTile(selectedUnit->tileX + 1, selectedUnit->tileY    , &unitsEnemy);
        MapTile tile4 = getTile(selectedUnit->tileX    , selectedUnit->tileY + 1, &unitsEnemy);

        //Does the tile have a player unit on it?
        bool tile1Empty = (getUnitAtTile(selectedUnit->tileX - 1, selectedUnit->tileY,     &unitsPlayer) == nullptr);
        bool tile2Empty = (getUnitAtTile(selectedUnit->tileX    , selectedUnit->tileY - 1, &unitsPlayer) == nullptr);
        bool tile3Empty = (getUnitAtTile(selectedUnit->tileX + 1, selectedUnit->tileY,     &unitsPlayer) == nullptr);
        bool tile4Empty = (getUnitAtTile(selectedUnit->tileX    , selectedUnit->tileY + 1, &unitsPlayer) == nullptr);

        if (tile1Empty && tile1.movementLostForMovingOnThisTile(selectedUnit->rescuedUnit) < 50)
        {
            menuChoices.push_back("Drop");
        }
        else if (tile2Empty && tile2.movementLostForMovingOnThisTile(selectedUnit->rescuedUnit) < 50)
        {
            menuChoices.push_back("Drop");
        }
        else if (tile3Empty && tile3.movementLostForMovingOnThisTile(selectedUnit->rescuedUnit) < 50)
        {
            menuChoices.push_back("Drop");
        }
        else if (tile4Empty && tile4.movementLostForMovingOnThisTile(selectedUnit->rescuedUnit) < 50)
        {
            menuChoices.push_back("Drop");
        }
    }

    menuChoices.push_back("Wait");
}

void Map::executeMenuChoice()
{
    if (menuChoices[menuIdx] == "Unit")
    {
        return;
    }
    else if (menuChoices[menuIdx] == "Status")
    {
        return;
    }
    else if (menuChoices[menuIdx] == "Options")
    {
        return;
    }
    else if (menuChoices[menuIdx] == "Suspend")
    {
        return;
    }
    else if (menuChoices[menuIdx] == "End")
    {
        turnChangeTimer = TURN_CHANGE_TIMER_MAX;
        mapState = PlayerPhaseEnding;
    }
    else if (menuChoices[menuIdx] == "Attack")
    {
        std::vector<Unit*> enemiesWeCanAttack = getEnemiesInRedTiles();
        if (enemiesWeCanAttack.size() > 0)
        {
            cursorX = enemiesWeCanAttack[0]->tileX;
            cursorY = enemiesWeCanAttack[0]->tileY;
        }
        mapState = UnitMenuAttackSelectTarget;
    }
    else if (menuChoices[menuIdx] == "Trade")
    {
        clearPreviewTiles();
        tradeSideLeft = true;
        tradeLeftIdx = 0;
        tradeIsLocked = false;

        std::vector<Unit*> units = getAdjacentUnits(selectedUnit, &unitsPlayer);
        if (units.size() > 0)
        {
            cursorX = units[0]->tileX;
            cursorY = units[0]->tileY;
        }

        mapState = UnitMenuTradeSelectTarget;
    }
    else if (menuChoices[menuIdx] == "Item")
    {
        itemIdx = 0;
        mapState = UnitMenuItem;
    }
    else if (menuChoices[menuIdx] == "Rescue")
    {
        return;
    }
    else if (menuChoices[menuIdx] == "Wait")
    {
        selectedUnit->isUsed = true;
        selectedUnit = nullptr;
        resetNeutralHudDescriptions();
        clearPreviewTiles();
        clearPreviewTilesEnemyAll();
        mapState = Neutral;
    }
    else if (menuChoices[menuIdx] == "Staff")
    {
        staffIdx = 0;
        mapState = UnitMenuStaffSelect;
    }

    menuIdx = 0;
    menuChoices.clear();
}

bool Map::unitCanUseStaff(Unit* unit, Item staff, std::vector<Unit*>* friends, std::vector<Unit*>* /*foes*/)
{
    if (unit->canUseStaff(staff))
    {
        if (staff.id == Heal ||
            staff.id == Mend ||
            staff.id == Recover)
        {
            std::vector<Unit*> units = getAdjacentUnits(unit, friends);
            for (int i = 0; i < units.size(); i++)
            {
                if (units[i]->hp < units[i]->maxHp)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void Map::renderMainMenu()
{
    int windowBoxX = 16;
    if ((cursorX - viewportX) < 7)
    {
        windowBoxX = 11*16;
    }
    WindowBox::render(windowBoxX, 16, 3*2, ((int)menuChoices.size() + 1)*2);
    renderHandCursor(windowBoxX, 16, menuIdx);

    for (int i = 0; i < menuChoices.size(); i++)
    {
        Text::renderText(menuChoices[i], Font::Border, Text::White, windowBoxX + 6, 24 + i*16, Left, 0);
    }
}

void Map::renderHandCursor(int windowX, int windowY, int idx)
{
    menuCursor->imageIndex++;
    menuCursor->x = windowX - 28;
    menuCursor->y = windowY + idx*16;
    menuCursor->render();
}

void Map::renderObjective()
{
    //objective
    int hudY = 0;
    if ((cursorY - viewportY) < 5 &&
        (cursorX - viewportX) >= 7)
    {
        hudY = -48*2;
    }

    hudObjectiveY = Util::approach(hudObjectiveY, hudY, 0.25f);
    int dispY = hudObjectiveY;
    if (dispY < -48)
    {
        dispY += 160 + 48;
    }

    hudObjectiveSprite->x = 240 - 96;
    hudObjectiveSprite->y = dispY;

    std::string line1 = "";
    std::string line2 = "";

    switch (objective)
    {
        case Rout:
            hudObjectiveSprite->imageIndex = 1;
            line1 = "Defeat Enemy";
            line2 = std::to_string(unitsEnemy.size()) + " Left";
            break;

        case Seize:
            hudObjectiveSprite->imageIndex = 0;
            line1 = "Seize";
            break;

        case Survive:
            hudObjectiveSprite->imageIndex = 1;
            line1 = "Survive";
            line2 = std::to_string(turnCount) + "/" + std::to_string(turnCountMax) + "Turn";
            break;

        case Protect:
            hudObjectiveSprite->imageIndex = 1;
            line1 = "Protect";
            line2 = std::to_string(turnCount) + "/" + std::to_string(turnCountMax) + "Turn";
            break;

        default:
            break;
    }

    hudObjectiveSprite->render();
    Text::renderText(line1, Font::Border, Text::White, hudObjectiveSprite->x + 14, dispY +  8, Center, 76);
    Text::renderText(line2, Font::Border, Text::White, hudObjectiveSprite->x + 14, dispY + 24, Center, 76);
}

void Map::renderTileDescription()
{
    //tile descriptor
    int hudX = 0;
    if ((cursorX - viewportX) < 7)
    {
        hudX = -48*2;
    }

    hudTileDescriptorX = Util::approach(hudTileDescriptorX, hudX, 0.25f);
    int dispX = hudTileDescriptorX;
    if (dispX < -48)
    {
        dispX += 240 + 48;
    }

    hudTileDescriptorSprite->x = dispX;
    hudTileDescriptorSprite->y = 160 - 54;
    hudTileDescriptorSprite->render();

    hudStatTexts->x = dispX + 8;
    hudStatTexts->y = 160 - 24;
    hudStatTexts->imageIndex = 1;
    hudStatTexts->render();

    hudStatTexts->x = dispX + 8;
    hudStatTexts->y = 160 - 16;
    hudStatTexts->imageIndex = 2;
    hudStatTexts->render();

    MapTile tile = tiles[cursorX + tilesWidth*cursorY];
    Text::renderText(tile.name, Font::Border, Text::White, dispX + 8, 160 - 40, Center, 32);

    Text::renderText(std::to_string(tile.defense), Font::Borderless, Text::DarkGrey, dispX + 24, 160 - 28, Right, 16);
    Text::renderText(std::to_string(tile.avoid),   Font::Borderless, Text::DarkGrey, dispX + 24, 160 - 20, Right, 16);
}

void Map::renderUnitDecription()
{
    int hudY = 0;
    if ((cursorX - viewportX) < 7 &&
        (cursorY - viewportY) < 5)
    {
        hudY = -48*2;
    }

    hudUnitDescriptorY = Util::approach(hudUnitDescriptorY, hudY, 0.25f);
    int dispY = hudUnitDescriptorY;
    if (dispY < -48)
    {
        dispY += 160 + 48;
    }

    Unit* unitToDisplay = getUnitAtTile(cursorX, cursorY, &unitsPlayer);
    if (unitToDisplay != nullptr)
    {
        hudUnitDescriptorSprite->imageIndex = 0;
    }
    else
    {
        unitToDisplay = getUnitAtTile(cursorX, cursorY, &unitsEnemy);
        hudUnitDescriptorSprite->imageIndex = 1;
    }

    if (unitToDisplay != nullptr)
    {
        hudUnitDescriptorSprite->x = 0;
        hudUnitDescriptorSprite->y = dispY;
        hudUnitDescriptorSprite->render();

        Text::renderText(unitToDisplay->unitResources.displayName, Font::Borderless, Text::DarkGrey, 40, dispY + 8, Center, 48);

        hudStatTexts->x = 40;
        hudStatTexts->y = dispY + 24;
        hudStatTexts->imageIndex = 0;
        hudStatTexts->render();

        std::string hpLine = std::to_string(unitToDisplay->hp) + "/" + std::to_string(unitToDisplay->maxHp);
        Text::renderText(hpLine, Font::Borderless, Text::DarkGrey, 56, dispY + 20, Center, 33);

        hudHpBar->x = 40;
        hudHpBar->y = dispY + 33;
        hudHpBar->render();

        if (unitToDisplay->maxHp != 0)
        {
            int hpWidth = (42*unitToDisplay->hp)/unitToDisplay->maxHp;
            SDL_Rect rect;
            rect.x = 43;
            rect.y = dispY + 35;
            rect.w = hpWidth;
            rect.h = 1;

            SDL_SetRenderDrawColor(Global::sdlRenderer, 255, 255, 255, 255);
            SDL_RenderFillRect(Global::sdlRenderer, &rect);

            rect.y = dispY + 36;
            SDL_SetRenderDrawColor(Global::sdlRenderer, 247, 239, 115, 255);
            SDL_RenderFillRect(Global::sdlRenderer, &rect);
        }

        unitToDisplay->sprMugshotTiny->x = 8;
        unitToDisplay->sprMugshotTiny->y = 8 + dispY;
        unitToDisplay->sprMugshotTiny->render();
    }
    else
    {
        hudUnitDescriptorY = -48;
    }
}

void Map::resetNeutralHudDescriptions()
{
    hudUnitDescriptorY = -48;
    hudTileDescriptorX = -48;
    hudObjectiveY = -48;
}

void Map::renderTurnChange(int imageIndex)
{
    float diff = (turnChangeTimer - (TURN_CHANGE_TIMER_MAX/2))/(0.5f*TURN_CHANGE_TIMER_MAX);
    float off = diff*diff*diff;
    turnChangeSprite->x = 24 + (int)(216*off);

    float tOpp = fabsf(off);
    tOpp = 1.0f - tOpp;
    SDL_SetRenderDrawColor(Global::sdlRenderer, 0, 0, 0, (Uint8)(128*tOpp));
    SDL_RenderFillRect(Global::sdlRenderer, nullptr);
    turnChangeSprite->imageIndex = imageIndex;
    turnChangeSprite->render({255, 255, 255, (Uint8)(255*tOpp)});
}

void Map::renderItemWindow(Unit* unit, int originX, int originY, int windowHeight, bool greyUnusables)
{
    WindowBox::render(originX, originY, 13, windowHeight*2 + 1);

    for (int i = 0; i < unit->items.size(); i++)
    {
        Item item = unit->items[i];
        item.render(originX + 4, originY + 4 + i*16);

        SDL_Color colorText = Text::White;
        SDL_Color colorUses = Text::Blue;
        if (greyUnusables)
        {
            if (item.isWeapon())
            {
                if (!unit->canUseWeapon(item))
                {
                    colorText = {180, 180, 180, 255};
                    colorUses = colorText;
                }
            }
            else if (item.isStaff())
            {
                if (!unit->canUseStaff(item))
                {
                    colorText = {180, 180, 180, 255};
                    colorUses = colorText;
                }
            }
            else if (!item.isConsumableByUnit(unit))
            {
                colorText = {180, 180, 180, 255};
                colorUses = colorText;
            }
        }
        Text::renderText(item.getName(), Font::Border, colorText, originX + 16 + 5, originY + 5 + i*16, Left, 0);
        Text::renderText(std::to_string(item.usesRemaining), Font::Border, colorUses, originX + 6*16 - 14, originY + 6 + i*16, Right, 16);
    }
}

void Map::renderItemWeaponStatsWindow()
{
    selectedUnit->sprMugshot->x = 8*16;
    selectedUnit->sprMugshot->y = 16;
    selectedUnit->sprMugshot->scaleX = 1;
    selectedUnit->sprMugshot->imageIndex++;
    selectedUnit->sprMugshot->render();

    WindowBox::render(8*16, 6*16, 6*2, 3*2);

    Text::renderText("Affi", Font::Border, Text::White, 10*16,     6*16 + 3, Left, 0);
    Text::renderText("Atk",  Font::Border, Text::White,  8*16 + 6, 7*16,     Left, 0);
    Text::renderText("Crit", Font::Border, Text::White, 11*16    , 7*16,     Left, 0);
    Text::renderText("Hit",  Font::Border, Text::White,  8*16 + 6, 8*16 - 2, Left, 0);
    Text::renderText("Avd",  Font::Border, Text::White, 11*16    , 8*16 - 2, Left, 0);

    int attack;
    int crit;
    int hit;
    int avoid;
    int attackSpeed;
    selectedUnit->calculateBaseCombatStats(&attack, &hit, &avoid, &crit, &attackSpeed);

    Text::renderText(std::to_string(attack), Font::Border, Text::Blue,  8*16 + 21, 7*16,     Right, 24);
    Text::renderText(std::to_string(crit),   Font::Border, Text::Blue, 11*16 + 17, 7*16,     Right, 24);
    Text::renderText(std::to_string(hit),    Font::Border, Text::Blue,  8*16 + 21, 8*16 - 2, Right, 24);
    Text::renderText(std::to_string(avoid),  Font::Border, Text::Blue, 11*16 + 17, 8*16 - 2, Right, 24);
}

void Map::renderItemEditWindow()
{
    const int ORIGIN_X = 88;
    const int ORIGIN_Y = 8;

    WindowBox::render(ORIGIN_X, ORIGIN_Y, 6, ((int)itemEditChoices.size() + 1)*2);
    menuCursor->imageIndex++;
    menuCursor->y = ORIGIN_Y + itemEditIdx*16;
    menuCursor->x = ORIGIN_X - 28;
    menuCursor->render();

    for (int i = 0; i < itemEditChoices.size(); i++)
    {
        Text::renderText(itemEditChoices[i], Font::Border, Text::White, ORIGIN_X + 8, ORIGIN_Y + 8 + i*16, Left, 0);
    }
}

void Map::renderStaffWindow()
{
    int originX = 12;
    int originY = 12;

    WindowBox::render(originX, originY, 13, (int)staffChoices.size()*2 + 1);

    for (int i = 0; i < staffChoices.size(); i++)
    {
        Item* item = staffChoices[i];
        item->render(originX + 4, originY + 4 + i*16);

        Text::renderText(item->getName(), Font::Border, Text::White, originX + 16 + 5, originY + 5 + i*16, Left, 0);
        Text::renderText(std::to_string(item->usesRemaining), Font::Border, Text::Blue, originX + 6*16 - 14, originY + 6 + i*16, Right, 16);
    }
}

void Map::renderStaffDescriptionWindow()
{
    selectedUnit->sprMugshot->x = 8*16;
    selectedUnit->sprMugshot->y = 16;
    selectedUnit->sprMugshot->scaleX = 1;
    selectedUnit->sprMugshot->imageIndex++;
    selectedUnit->sprMugshot->render();

    WindowBox::render(8*16, 6*16, 6*2, 3*2);
}

void Map::renderUnits(std::vector<Unit*>* units, int spriteIndex, Unit* ignoreMe)
{
    for (int i = 0; i < units->size(); i++)
    {
        Unit* unit = units->at(i);
        if (unit != ignoreMe)
        {
            unit->render(unit->tileX*16, unit->tileY*16, spriteIndex, viewportPixelX, viewportPixelY);
        }
    }
}

void Map::renderCursor()
{
    cursor->x = viewportPixelX + cursorX*16;
    cursor->y = viewportPixelY + cursorY*16;
    cursor->imageIndex++;
    cursor->render();
}

std::vector<SDL_Point> Map::calculatePreviewTilesAtTile(int tileX, int tileY, std::unordered_set<int> ranges)
{
    std::vector<SDL_Point> tileLocations;

    for (auto itr = ranges.begin(); itr != ranges.end(); ++itr)
    {
        int range = *itr;
        for (int y = -range; y <= range; y++)
        {
            for (int x = -range; x <= range; x++)
            {
                if (abs(y) + abs(x) == range)
                {
                    int globalX = tileX + x;
                    int globalY = tileY + y;

                    if (globalX < 0 || globalX >= tilesWidth ||
                        globalY < 0 || globalY >= tilesHeight)
                    {
                        continue;
                    }

                    MapTile tile = tiles[globalX + tilesWidth*globalY];
                    if (tile.type != NoPass)
                    {
                        tileLocations.push_back({globalX, globalY});
                    }
                }
            }
        }
    }

    return tileLocations;
}

void Map::createAttackPreviewTiles(Unit* unit, std::unordered_set<int> ranges)
{
    std::vector<SDL_Point> reds = calculatePreviewTilesAtTile(unit->tileX, unit->tileY, ranges);

    for (auto itr = reds.begin(); itr != reds.end(); ++itr)
    {
        SDL_Point newRedTile = *itr;

        int x = newRedTile.x;
        int y = newRedTile.y;
        Map::previewTilesRed.push_back(new Sprite("res/Images/Sprites/Map/PreviewTileRed", x, y, false));
    }
}

void Map::createStaffPreviewTiles(Unit* unit, std::unordered_set<int> ranges)
{
    std::vector<SDL_Point> greens = calculatePreviewTilesAtTile(unit->tileX, unit->tileY, ranges);

    for (auto itr = greens.begin(); itr != greens.end(); ++itr)
    {
        SDL_Point newGreenTile = *itr;

        int x = newGreenTile.x;
        int y = newGreenTile.y;
        Map::previewTilesGreen.push_back(new Sprite("res/Images/Sprites/Map/PreviewTileGreen", x, y, false));
    }
}

void Map::renderAttackPreview()
{
    Unit* enemyOnCursor = getUnitAtTile(cursorX, cursorY, &unitsEnemy);
    if (enemyOnCursor != nullptr)
    {
        bool unitIsInRedTile = false;
        for (int i = 0; i < previewTilesRed.size(); i++)
        {
            if (enemyOnCursor->tileX == previewTilesRed[i]->x &&
                enemyOnCursor->tileY == previewTilesRed[i]->y)
            {
                unitIsInRedTile = true;
                break;
            }
        }

        if (unitIsInRedTile)
        {
            int baseX = 0;
            int baseY = 0;

            if ((cursorX - viewportX) < 7)
            {
                baseX = 240 - 88;
            }

            attackPreviewBackdrop->x = baseX;
            attackPreviewBackdrop->y = baseY;
            attackPreviewBackdrop->render();

            Text::renderText(selectedUnit ->unitResources.displayName, Font::Border, Text::White, baseX + 26, baseY +  9, Center, 42);
            Text::renderText(enemyOnCursor->unitResources.displayName, Font::Border, Text::White, baseX + 10, baseY + 89, Center, 42);

            int myAttackSpeed = selectedUnit->spd;
            Item* myWeapon = selectedUnit->getEquippedWeapon();
            if (myWeapon != nullptr)
            {
                myAttackSpeed = selectedUnit->getAttackSpeedWithWeapon(myWeapon->getWeaponStats());

                myWeapon->render(baseX + 8, baseY + 8);
            }
            
            int otherAttackSpeed = enemyOnCursor->spd;
            Item* otherWeapon = enemyOnCursor->getEquippedWeapon();
            if (otherWeapon != nullptr)
            {
                otherAttackSpeed = enemyOnCursor->getAttackSpeedWithWeapon(otherWeapon->getWeaponStats());

                otherWeapon->render(baseX + 56, baseY + 90);
                Text::renderText(otherWeapon->getName(), Font::Border, Text::White, baseX + 10, baseY + 105, Center, 50);
            }

            int myAttack    = 0;
            int myHit       = 0;
            int myCrit      = 0;
            int otherAttack = 0;
            int otherHit    = 0;
            int otherCrit   = 0;
            selectedUnit ->calculateCombatStatsVsUnit(enemyOnCursor, &myAttack,    &myHit,    &myCrit);
            enemyOnCursor->calculateCombatStatsVsUnit(selectedUnit,  &otherAttack, &otherHit, &otherCrit);

            Text::renderText(std::to_string(selectedUnit->hp),  Font::Border, Text::Blue, baseX + 48, baseY + 25, Right, 24);
            Text::renderText(std::to_string(enemyOnCursor->hp), Font::Border, Text::Blue, baseX +  2, baseY + 25, Right, 24);
            Text::renderText(std::to_string(myAttack),          Font::Border, Text::Blue, baseX + 48, baseY + 41, Right, 24);
            Text::renderText(std::to_string(otherAttack),       Font::Border, Text::Blue, baseX +  2, baseY + 41, Right, 24);
            Text::renderText(std::to_string(myHit),             Font::Border, Text::Blue, baseX + 48, baseY + 57, Right, 24);
            Text::renderText(std::to_string(otherHit),          Font::Border, Text::Blue, baseX +  2, baseY + 57, Right, 24);
            Text::renderText(std::to_string(myCrit),            Font::Border, Text::Blue, baseX + 48, baseY + 73, Right, 24);
            Text::renderText(std::to_string(otherCrit),         Font::Border, Text::Blue, baseX +  2, baseY + 73, Right, 24);

            if (myAttackSpeed - 4 >= otherAttackSpeed)
            {
                attackPreviewMultiplier->x = baseX + 70 + (int)(3*sinf(Global::frameCount*0.1f));
                attackPreviewMultiplier->y = baseY + 41 + (int)(3*cosf(Global::frameCount*0.1f));
                attackPreviewMultiplier->render();
            }
            else if (otherAttackSpeed - 4 >= myAttackSpeed)
            {
                attackPreviewMultiplier->x = baseX + 24 + (int)(3*sinf(Global::frameCount*0.1f));
                attackPreviewMultiplier->y = baseY + 41 + (int)(3*cosf(Global::frameCount*0.1f));
                attackPreviewMultiplier->render();
            }
        }
    }
}
