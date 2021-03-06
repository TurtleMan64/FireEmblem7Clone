#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <string>
#include <vector>
#include <unordered_set>

#include "Global.hpp"
#include "Sprite.hpp"
#include "Util.hpp"
#include "Input.hpp"
#include "Items.hpp"
#include "Weapons.hpp"
#include "WindowBox.hpp"
#include "Text.hpp"
#include "MapTile.hpp"
#include "Map.hpp"


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


Sprite* Map::attackPreviewBackdrop = nullptr;

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

Sprite* Map::previewArrowSprite = nullptr;

int Map::turnCount = 0;
int Map::turnCountMax = 99;

MapObjective Map::objective = Rout;
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

int* Map::dijkstraGraph         = nullptr;
int* Map::dijkstraCost          = nullptr;
int* Map::dijkstraTilesDistance = nullptr;
int* Map::dijkstraTilesPath     = nullptr;

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

        player->weaponRank[Sword] = (WeaponRank)line[21][0];
        player->weaponRank[Lance] = (WeaponRank)line[22][0];
        player->weaponRank[Axe  ] = (WeaponRank)line[23][0];
        player->weaponRank[Bow  ] = (WeaponRank)line[24][0];
        player->weaponRank[Anima] = (WeaponRank)line[25][0];
        player->weaponRank[Light] = (WeaponRank)line[26][0];
        player->weaponRank[Dark ] = (WeaponRank)line[27][0];
        player->weaponRank[Staff] = (WeaponRank)line[28][0];

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

        enemy->weaponRank[Sword] = (WeaponRank)line[21][0];
        enemy->weaponRank[Lance] = (WeaponRank)line[22][0];
        enemy->weaponRank[Axe  ] = (WeaponRank)line[23][0];
        enemy->weaponRank[Bow  ] = (WeaponRank)line[24][0];
        enemy->weaponRank[Anima] = (WeaponRank)line[25][0];
        enemy->weaponRank[Light] = (WeaponRank)line[26][0];
        enemy->weaponRank[Dark ] = (WeaponRank)line[27][0];
        enemy->weaponRank[Staff] = (WeaponRank)line[28][0];

        unitsEnemy.push_back(enemy);
    }

    if (cursor == nullptr)
    {
        cursor = new Sprite("res/Images/Sprites/Map/Cursor", 0, 0, false);
    }

    if (previewArrowSprite == nullptr)
    {
        previewArrowSprite = new Sprite("res/Images/Sprites/Map/PreviewArrow", 0, 0, false);
    }

    if (menuCursor == nullptr)
    {
        menuCursor = new Sprite("res/Images/Sprites/Window/Hand", 0, 0, false);
    }

    if (hudObjectiveSprite == nullptr)
    {
        hudObjectiveSprite = new Sprite("res/Images/Sprites/Map/ObjectiveDisplay", 0, 0, false);
    }

    if (hudTileDescriptorSprite == nullptr)
    {
        hudTileDescriptorSprite = new Sprite("res/Images/Sprites/Map/TileDisplay", 0, 0, false);
    }

    if (hudUnitDescriptorSprite == nullptr)
    {
        hudUnitDescriptorSprite = new Sprite("res/Images/Sprites/Map/UnitDisplay", 0, 0, false);
    }

    if (hudStatTexts == nullptr)
    {
        hudStatTexts = new Sprite("res/Images/Sprites/Map/StatTexts", 0, 0, false);
    }

    if (hudHpBar == nullptr)
    {
        hudHpBar = new Sprite("res/Images/Sprites/Map/HpBar", 0, 0, false);
    }

    if (turnChangeSprite == nullptr)
    {
        turnChangeSprite = new Sprite("res/Images/Sprites/Map/TurnChange", 0, 64, false);
    }

    if (attackPreviewBackdrop == nullptr)
    {
        attackPreviewBackdrop = new Sprite("res/Images/Sprites/Map/PreviewAttack", 0, 0, false);
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

        renderUnits(&unitsEnemy,  6, nullptr);
        renderUnits(&unitsPlayer, 0, useableUnitOnCursor);
        if (useableUnitOnCursor != nullptr)
        {
            useableUnitOnCursor->render(useableUnitOnCursor->tileX*16, useableUnitOnCursor->tileY*16, 1, viewportPixelX, viewportPixelY);
        }

        renderCursor();
        renderTileDescription();
        renderObjective();
        renderUnitDecription();

        if (Input::pressedA())
        {
            if (useableUnitOnCursor != nullptr)
            {
                selectedUnit = useableUnitOnCursor;
                selectedUnitOriginalTileX = selectedUnit->tileX;
                selectedUnitOriginalTileY = selectedUnit->tileY;
                walkingPath.clear();
                clearPreviewTiles();
                calculatePreviewTiles(selectedUnit);
                mapState = MovingUnit;
                selectedUnitCanGoBack = true;
            }
            else
            {
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

        break;
    }

    case NeutralMenu:
    {
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
            resetNeutralHudDescriptions();
            mapState = Neutral;
        }

        break;
    }

    case MovingUnit:
    {
        updateCursor();

        renderPreviewTiles();

        if (Input::pressedB())
        {
            clearPreviewTiles();

            selectedUnit = nullptr;
            resetNeutralHudDescriptions();
            mapState = Neutral;
        }
        else if (unitCanMoveToTile(selectedUnit, cursorX, cursorY))
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
                        generateWalkingPath(selectedUnit, cursorX, cursorY);

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

                clearPreviewTiles();

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

        renderItemWindow(selectedUnit, 16, 16, (int)selectedUnit->items.size());
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

            if (selectedUnit->items[itemIdx].isWeapon())
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

        renderItemWindow(selectedUnit, 16, 16, (int)selectedUnit->items.size());
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
                Item itemAt0 = selectedUnit->items[0];
                Item itemAtIdx = selectedUnit->items[itemIdx];

                selectedUnit->items[0] = itemAtIdx;
                selectedUnit->items[itemIdx] = itemAt0;

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
                enemyOnCursor->hp--;
                selectedUnit->hp--;
                selectedUnit->isUsed = true;
                selectedUnit = nullptr;
                resetNeutralHudDescriptions();
                mapState = Neutral;
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

        renderItemWindow(selectedUnit, 12,   16*4 + 8, 5);
        renderItemWindow(tradingUnit,  16*7 + 12, 16*4 + 8, 5);

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

    case PlayerPhaseEnding:
    {
        turnChangeTimer--;

        resetNeutralHudDescriptions();
        renderUnits(&unitsEnemy,  6, nullptr);
        renderUnits(&unitsPlayer, 0, nullptr);

        if (turnChangeTimer <= 0)
        {
            isPhaseEnemy  = true;
            isPhasePlayer = false;

            for (int i = 0; i < unitsPlayer.size(); i++)
            {
                unitsPlayer[i]->isUsed = false;
            }

            mapState = EnemyPhaseEnding;
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
    //Render all of the blue units
    for (int i = 0; i < unitsPlayer.size(); i++)
    {
        unitsPlayer[i]->render(unitsPlayer[i]->tileX*16, unitsPlayer[i]->tileY*16, 0, viewportPixelX, viewportPixelY);
    }

    //Render the red units
    for (int i = 0; i < unitsEnemy.size(); i++)
    {
        unitsEnemy[i]->render(unitsEnemy[i]->tileX*16, unitsEnemy[i]->tileY*16, 6, viewportPixelX, viewportPixelY);
    }

    if (turnChangeTimer == 0)
    {
        turnChangeTimer = TURN_CHANGE_TIMER_MAX;
    }
    else
    {
        turnChangeTimer--;

        if (turnChangeTimer == 0)
        {
            isPhaseEnemy  = false;
            isPhasePlayer = true;
            mapState = Neutral;
        }
        else
        {
            renderTurnChange(0);
        }
    }
}

void Map::updateCursor()
{
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
}

void Map::generateWalkingPath(Unit* unit, int tileX, int tileY)
{
    walkingPath.clear();
    walkingPath.push_back(SDL_Point{tileX, tileY});

    if (unit->tileX == tileX && unit->tileY == tileY)
    {
        return;
    }

    int nodeX = (tileX - unit->tileX) + MAX_MOVEMENT;
    int nodeY = (tileY - unit->tileY) + MAX_MOVEMENT;
    int endNode = nodeX + nodeY*NODES_WIDTH;
    int currentNode = endNode;
    do
    {
        currentNode = dijkstraTilesPath[currentNode];

        int thisNodeX = (currentNode % NODES_WIDTH) - MAX_MOVEMENT;
        int thisNodeY = (currentNode / NODES_WIDTH) - MAX_MOVEMENT;
        SDL_Point thisTile {unit->tileX + thisNodeX, unit->tileY + thisNodeY};
        walkingPath.push_back(thisTile);
        
    } while(currentNode != NUM_NODES/2);
}

bool Map::unitCanMoveToTile(Unit* unit, int tileX, int tileY)
{
    if (unit->tileX == tileX && unit->tileY == tileY)
    {
        return true;
    }

    int nodeX = (tileX - unit->tileX) + MAX_MOVEMENT;
    int nodeY = (tileY - unit->tileY) + MAX_MOVEMENT;

    if (nodeX < 0 || nodeY < 0 || nodeX >= NODES_WIDTH || nodeY >= NODES_WIDTH)
    {
        return false;
    }

    return (dijkstraTilesDistance[nodeX + nodeY*NODES_WIDTH] <= unit->mov);
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

MapTile Map::getTile(int x, int y)
{
    if (x < 0 || y < 0 || x >= tilesWidth || y >= tilesHeight || (getUnitAtTile(x, y, &unitsEnemy) != nullptr))
    {
        return MapTile(MapTileType::NoPass, "OutOfBounds");
    }

    return tiles[x + tilesWidth*y];
}

void Map::constructGraph(Unit* unit, int* graph)
{
    int startTileX = unit->tileX - MAX_MOVEMENT;
    int startTileY = unit->tileY - MAX_MOVEMENT;

    for (int node = 0; node < NUM_NODES; node++)
    {
        int currentTileX = node % NODES_WIDTH;
        int currentTileY = node / NODES_WIDTH;

        MapTile tileUp    = getTile(startTileX + currentTileX,     startTileY + currentTileY - 1);
        MapTile tileDown  = getTile(startTileX + currentTileX,     startTileY + currentTileY + 1);
        MapTile tileLeft  = getTile(startTileX + currentTileX - 1, startTileY + currentTileY);
        MapTile tileRight = getTile(startTileX + currentTileX + 1, startTileY + currentTileY);

        int nodeUp    = node - NODES_WIDTH;
        int nodeDown  = node + NODES_WIDTH;
        int nodeLeft  = node - 1;
        int nodeRight = node + 1;

        bool upIsValid    = ((node - NODES_WIDTH) >= 0);
        bool downIsValid  = ((node + NODES_WIDTH) <  NUM_NODES);
        bool leftIsValid  = ((node % NODES_WIDTH) != 0);
        bool rightIsValid = ((node % NODES_WIDTH) != NODES_WIDTH - 1);

        if (upIsValid   ) { graph[node*NUM_NODES + nodeUp   ] = tileUp   .movementLostForMovingOnThisTile(unit); }
        if (downIsValid ) { graph[node*NUM_NODES + nodeDown ] = tileDown .movementLostForMovingOnThisTile(unit); }
        if (leftIsValid ) { graph[node*NUM_NODES + nodeLeft ] = tileLeft .movementLostForMovingOnThisTile(unit); }
        if (rightIsValid) { graph[node*NUM_NODES + nodeRight] = tileRight.movementLostForMovingOnThisTile(unit); }
    }
}

void Map::calculatePreviewTiles(Unit* unit)
{
    if (dijkstraGraph == nullptr)
    {
        dijkstraGraph = new int[NUM_NODES*NUM_NODES];
    }
    memset(dijkstraGraph, 0, sizeof(int)*NUM_NODES*NUM_NODES);

    Map::constructGraph(unit, dijkstraGraph);

    if (dijkstraCost == nullptr)
    {
        dijkstraCost = new int[NUM_NODES*NUM_NODES];
    }
    memset(dijkstraCost, 0, sizeof(int)*NUM_NODES*NUM_NODES);
    
    if (dijkstraTilesDistance == nullptr)
    {
        dijkstraTilesDistance = new int[NUM_NODES];
    }
    memset(dijkstraTilesDistance, 0, sizeof(int)*NUM_NODES);

    if (dijkstraTilesPath == nullptr)
    {
        dijkstraTilesPath = new int[NUM_NODES];
    }
    memset(dijkstraTilesPath, 0, sizeof(int)*NUM_NODES);

    const int INF = 100000000;

    int visited[NUM_NODES], count, mindistance, nextnode = 0, i, j;
    for (i = 0; i < NUM_NODES; i++)
    {
        for (j = 0; j < NUM_NODES; j++)
        {
            if (dijkstraGraph[i*NUM_NODES + j] == 0)
            {
                dijkstraCost[i*NUM_NODES + j] = INF;
            }
            else
            {
                dijkstraCost[i*NUM_NODES + j] = dijkstraGraph[i*NUM_NODES + j];
            }
        }
    }

    const int START_NODE = NUM_NODES/2;
    for (i = 0; i < NUM_NODES; i++)
    {
        dijkstraTilesDistance[i] = dijkstraCost[START_NODE*NUM_NODES + i];
        dijkstraTilesPath[i] = START_NODE;
        visited[i] = 0;
    }
    dijkstraTilesDistance[START_NODE] = 0;
    visited[START_NODE] = 1;
    count = 1;
    while (count < NUM_NODES - 1)
    {
        mindistance = INF;
        for (i = 0; i < NUM_NODES; i++)
        {
            if (dijkstraTilesDistance[i] < mindistance && !visited[i])
            {
                mindistance = dijkstraTilesDistance[i];
                nextnode = i;
            }
        }
        visited[nextnode] = 1;
        for (i = 0; i < NUM_NODES; i++)
        {
            if (!visited[i])
            {
                if (mindistance + dijkstraCost[nextnode*NUM_NODES + i] < dijkstraTilesDistance[i])
                {
                    dijkstraTilesDistance[i] = mindistance + dijkstraCost[nextnode*NUM_NODES + i];
                    dijkstraTilesPath[i] = nextnode;
                }
            }
        }
        count++;
    }

    std::unordered_set<int> blueTiles;

    for (i = 0; i < NUM_NODES; i++)
    {
        //printf("Distance of node %d = %d\n", i, distance[i]);
        if (dijkstraTilesDistance[i] <= unit->mov)
        {
            int nodeX = (i % Map::NODES_WIDTH) - (Map::NODES_WIDTH/2);
            int nodeY = (i / Map::NODES_WIDTH) - (Map::NODES_WIDTH/2);

            int tileX = Map::selectedUnit->tileX + nodeX;
            int tileY = Map::selectedUnit->tileY + nodeY;

            Map::previewTilesBlue.push_back(new Sprite("res/Images/Sprites/Map/PreviewTileBlue", tileX, tileY, false));
            blueTiles.insert(tileX | (tileY << 16));
        }
        //printf("Path = %d", i);
        j = i;
        do
        {
            j = dijkstraTilesPath[j];
            //printf("<-%d", j);
        } while(j != START_NODE);
        //printf("\n");
    }

    std::unordered_set<int> attackRanges = unit->getAttackRanges();
    std::unordered_set<int> redTiles;

    for (int t = 0; t < previewTilesBlue.size(); t++)
    {
        int baseX = previewTilesBlue[t]->x;
        int baseY = previewTilesBlue[t]->y;

        Unit* blueAtTile = getUnitAtTile(baseX, baseY, &unitsPlayer);
        Unit* redAtTile  = getUnitAtTile(baseX, baseY, &unitsEnemy);

        if ((blueAtTile != unit && blueAtTile != nullptr) || redAtTile != nullptr)
        {
            continue;
        }

        std::unordered_set<int> newReds = calculateRedTilesAtTile(baseX, baseY, attackRanges);
        redTiles.insert(newReds.begin(), newReds.end());
    }

    for (auto itr = redTiles.begin(); itr != redTiles.end(); ++itr)
    {
        int newRedTile = *itr;

        auto contains = blueTiles.find(newRedTile);
        if (contains == blueTiles.end()) //Dont create if already a blue tile
        {
            int x = newRedTile & 0xFFFF;
            int y = (newRedTile >> 16) & 0xFFFF;
            Map::previewTilesRed.push_back(new Sprite("res/Images/Sprites/Map/PreviewTileRed", x, y, false));
        }
    }
}

void Map::renderPreviewArrows()
{
    generateWalkingPath(selectedUnit, cursorX, cursorY);

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
        MapTile tile1 = getTile(selectedUnit->tileX - 1, selectedUnit->tileY);
        MapTile tile2 = getTile(selectedUnit->tileX    , selectedUnit->tileY - 1);
        MapTile tile3 = getTile(selectedUnit->tileX + 1, selectedUnit->tileY);
        MapTile tile4 = getTile(selectedUnit->tileX    , selectedUnit->tileY + 1);

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
        mapState = Neutral;
    }

    menuIdx = 0;
    menuChoices.clear();
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
        Text::renderText(menuChoices[i], Font::White, {255, 255, 255, 255}, windowBoxX + 6, 24 + i*16, false, 0, 0);
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
    Text::renderText(line1, Font::White, {255, 255, 255, 255}, hudObjectiveSprite->x + 14, dispY +  8, true, 76, 0);
    Text::renderText(line2, Font::White, {255, 255, 255, 255}, hudObjectiveSprite->x + 14, dispY + 24, true, 76, 0);
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
    Text::renderText(tile.name, Font::White, {255, 255, 255, 255}, dispX + 8, 160 - 40, true, 32, 0);

    Text::renderText(std::to_string(tile.defense), Font::Black, {255, 255, 255, 255}, dispX + 25, 160 - 28, false, 0, 2);
    Text::renderText(std::to_string(tile.avoid),   Font::Black, {255, 255, 255, 255}, dispX + 25, 160 - 20, false, 0, 2);
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

        Text::renderText(unitToDisplay->displayName, Font::Black, {255, 255, 255, 255}, 40, dispY + 8, true, 48, 0);

        hudStatTexts->x = 40;
        hudStatTexts->y = dispY + 24;
        hudStatTexts->imageIndex = 0;
        hudStatTexts->render();

        std::string hpLine = std::to_string(unitToDisplay->hp) + "/" + std::to_string(unitToDisplay->maxHp);
        Text::renderText(hpLine, Font::Black, {255, 255, 255, 255}, 56, dispY + 20, true, 33, 0);

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

void Map::renderItemWindow(Unit* unit, int originX, int originY, int windowHeight)
{
    WindowBox::render(originX, originY, 13, windowHeight*2 + 1);

    for (int i = 0; i < unit->items.size(); i++)
    {
        Item item = unit->items[i];
        item.render(originX + 4, originY + 4 + i*16);
        Text::renderText(item.getName(), Font::White, {255, 255, 255, 255}, originX + 16 + 5, originY + 5 + i*16, false, 0, 0);
        Text::renderText(std::to_string(item.usesRemaining), Font::White, {198, 255, 255, 255}, originX + 6*16 - 11, originY + 6 + i*16, false, 0, 2);
    }
}

void Map::renderItemWeaponStatsWindow()
{
    selectedUnit->sprMugshot->x = 8*16;
    selectedUnit->sprMugshot->y = 16;
    selectedUnit->sprMugshot->imageIndex++;
    selectedUnit->sprMugshot->render();

    WindowBox::render(8*16, 6*16, 6*2, 3*2);

    Text::renderText("Affi", Font::White, {255, 255, 255, 255}, 10*16,     6*16 + 3, false, 0, 0);
    Text::renderText("Atk",  Font::White, {255, 255, 255, 255},  8*16 + 6, 7*16,     false, 0, 0);
    Text::renderText("Crit", Font::White, {255, 255, 255, 255}, 11*16    , 7*16,     false, 0, 0);
    Text::renderText("Hit",  Font::White, {255, 255, 255, 255},  8*16 + 6, 8*16 - 2, false, 0, 0);
    Text::renderText("Avd",  Font::White, {255, 255, 255, 255}, 11*16    , 8*16 - 2, false, 0, 0);

    Item* weapon = selectedUnit->getEquippedWeapon();
    WeaponStats stats = weapon->getWeaponStats();
    MapTile tile = tiles[selectedUnit->tileX + selectedUnit->tileY*tilesWidth];

    //Attack calculation
    int supportBonusAttack = 0;
    int attack = stats.might + selectedUnit->str + supportBonusAttack;

    //Crit calculation
    int supportBonusCrit = 0;
    int criticalBonus = 0;
    int sRankBonusCrit = 0;
    if (selectedUnit->weaponRank[stats.type] == S)
    {
        sRankBonusCrit = 5;
    }
    int crit = stats.crit + selectedUnit->skl/2 + supportBonusCrit + criticalBonus + sRankBonusCrit;

    //Hit calculation
    int supportBonusHit = 0;
    int sRankBonusHit = 0;
    if (selectedUnit->weaponRank[stats.type] == S)
    {
        sRankBonusHit = 5;
    }
    int tacticanBonusHit = 0;
    int hit = stats.hit + selectedUnit->skl*2 + selectedUnit->lck/2 + supportBonusHit + sRankBonusHit + tacticanBonusHit;

    //Avoid calculation
    int supportBonusAvoid = 0;
    int terrainBonusAvoid = tile.avoid;
    if (selectedUnit->classType == ClassType::Pegasus ||
        selectedUnit->classType == ClassType::Wyvern)
    {
        terrainBonusAvoid = 0;
    }
    int tacticanBonusAvoid = 0;
    int avoid = selectedUnit->getAttackSpeedWithWeapon(*weapon) + selectedUnit->lck + supportBonusAvoid + terrainBonusAvoid + tacticanBonusAvoid;

    Text::renderText(std::to_string(attack), Font::White, {198, 255, 255, 255},  8*16 + 6 + 17, 7*16,     false, 0, 3);
    Text::renderText(std::to_string(crit),   Font::White, {198, 255, 255, 255}, 11*16     + 19, 7*16,     false, 0, 3);
    Text::renderText(std::to_string(hit),    Font::White, {198, 255, 255, 255},  8*16 + 6 + 17, 8*16 - 2, false, 0, 3);
    Text::renderText(std::to_string(avoid),  Font::White, {198, 255, 255, 255}, 11*16     + 19, 8*16 - 2, false, 0, 3);
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
        Text::renderText(itemEditChoices[i], Font::White, {255, 255, 255, 255}, ORIGIN_X + 8, ORIGIN_Y + 8 + i*16, false, 0, 0);
    }
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

std::unordered_set<int> Map::calculateRedTilesAtTile(int tileX, int tileY, std::unordered_set<int> ranges)
{
    std::unordered_set<int> reds;

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
                        reds.insert(globalX | (globalY << 16));
                    }
                }
            }
        }
    }

    return reds;
}

void Map::createAttackPreviewTiles(Unit* unit, std::unordered_set<int> ranges)
{
    std::unordered_set<int> reds = calculateRedTilesAtTile(unit->tileX, unit->tileY, ranges);

    for (auto itr = reds.begin(); itr != reds.end(); ++itr)
    {
        int newRedTile = *itr;

        int x = (newRedTile >>  0) & 0xFFFF;
        int y = (newRedTile >> 16) & 0xFFFF;
        Map::previewTilesRed.push_back(new Sprite("res/Images/Sprites/Map/PreviewTileRed", x, y, false));
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
            }
        }

        if (unitIsInRedTile)
        {
            int baseX = 0;
            int baseY = 0;
            attackPreviewBackdrop->x = baseX;
            attackPreviewBackdrop->y = baseY;
            attackPreviewBackdrop->render();

            Text::renderText(selectedUnit ->displayName, White, {255,255,255,255}, baseX + 26, baseY +  9, true, 42, 0);
            Text::renderText(enemyOnCursor->displayName, White, {255,255,255,255}, baseX + 10, baseY + 89, true, 42, 0);
        }
    }
}
