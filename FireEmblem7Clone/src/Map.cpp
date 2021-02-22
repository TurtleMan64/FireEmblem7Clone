#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <string>
#include <vector>

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

int Map::viewportX = 0;
int Map::viewportY = 0;
int Map::viewportPixelX = 0;
int Map::viewportPixelY = 0;

Sprite* Map::cursor = nullptr;
int Map::cursorX = 0;
int Map::cursorY = 0;

int Map::walkingTimer = 0;
bool Map::hasTemporarilyWalked = false;
std::vector<SDL_Point> Map::walkingPath;

Sprite* Map::menuCursor = nullptr;
int Map::menuIdx = 0;
std::vector<std::string> Map::menuChoices;

int  Map::itemIdx = 0;
bool Map::isViewingItems = false;

int  Map::itemEditIdx = 0;
std::vector<std::string> Map::itemEditChoices;

bool Map::hasSelectedBlankTile = false;

Sprite* Map::turnChangeSprite = nullptr;
int Map::turnChangeTimer = 0;
bool Map::isPhasePlayer = true;
bool Map::isPhaseEnemy = true;

Unit* Map::selectedUnit = nullptr;
int Map::selectedUnitOriginalTileX = 0;
int Map::selectedUnitOriginalTileY = 0;
std::vector<Sprite*> Map::previewBlueTiles;

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

    //Player phase logic
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
    updateCursor();

    if ((hasTemporarilyWalked || hasSelectedBlankTile) && turnChangeTimer == 0 && !isViewingItems)
    {
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
    }

    if (isViewingItems)
    {
        if (selectedUnit->items.size() == 0)
        {
            isViewingItems = false;
            itemEditChoices.clear();
            calculateMenuChoices();
        }
        else
        {
            if (itemEditChoices.size() > 0)
            {
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
            }
            else
            {
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
            }
        }
    }

    if (walkingTimer > 0)
    {
        if (walkingTimer == 1)
        {
            hasTemporarilyWalked = true;
            selectedUnit->tileX = cursorX;
            selectedUnit->tileY = cursorY;
            calculateMenuChoices();
        }
        walkingTimer--;
    }

    if (Input::pressedB() && walkingTimer == 0 && turnChangeTimer == 0)
    {
        if (isViewingItems)
        {
            if (itemEditChoices.size() > 0)
            {
                itemEditChoices.clear();
            }
            else
            {
                isViewingItems = false;
                calculateMenuChoices();
            }
        }
        else
        {
            if (selectedUnit != nullptr)
            {
                selectedUnit->tileX = selectedUnitOriginalTileX;
                selectedUnit->tileY = selectedUnitOriginalTileY;

                clearBlueTiles();

                selectedUnit = nullptr;
                hasTemporarilyWalked = false;
            }

            if (hasSelectedBlankTile)
            {
                hasSelectedBlankTile = false;
                menuIdx = 0;
                menuChoices.clear();
            }
        }
    }
        
    if (Input::pressedA() && walkingTimer == 0 && turnChangeTimer == 0)
    {
        if (isViewingItems)
        {
            if (itemEditChoices.size() == 0)
            {
                if (selectedUnit->items[itemIdx].isWeapon())
                {
                    itemEditChoices.push_back("Equip");
                }

                if (selectedUnit->items[itemIdx].isConsumableByUnit(selectedUnit))
                {
                    itemEditChoices.push_back("Use");
                }

                itemEditChoices.push_back("Discard");
            }
            else
            {
                if (itemEditChoices[itemEditIdx] == "Equip")
                {
                    //swap
                    Item itemAt0 = selectedUnit->items[0];
                    Item itemAtIdx = selectedUnit->items[itemIdx];

                    selectedUnit->items[0] = itemAtIdx;
                    selectedUnit->items[itemIdx] = itemAt0;

                    itemIdx = 0;
                    itemEditIdx = 0;
                    itemEditChoices.clear();
                }
                else if (itemEditChoices[itemEditIdx] == "Discard")
                {
                    selectedUnit->items.erase(selectedUnit->items.begin() + itemIdx);

                    itemEditIdx = 0;
                    itemEditChoices.clear();
                    isViewingItems = false;
                    calculateMenuChoices();
                }
                else if (itemEditChoices[itemEditIdx] == "Use")
                {
                    selectedUnit->items[itemIdx].consume(selectedUnit);
                    selectedUnit->items[itemIdx].usesRemaining--;

                    if (selectedUnit->items[itemIdx].usesRemaining == 0)
                    {
                        selectedUnit->items.erase(selectedUnit->items.begin() + itemIdx);
                    }

                    hasTemporarilyWalked = false;
                    //menuChoices.clear();
                    isViewingItems = false;
                    selectedUnit->isUsed = true;
                    selectedUnit = nullptr;
                }
            }
        }
        else
        {
            if (hasSelectedBlankTile)
            {
                executeMenuChoice();
            }
            else if (selectedUnit == nullptr)
            {
                //Are we clicking on a blue unit?
                bool clickedOnABlueUnit = false;
                for (int i = 0; i < unitsPlayer.size(); i++)
                {
                    if (unitsPlayer[i]->tileX == cursorX &&
                        unitsPlayer[i]->tileY == cursorY &&
                        unitsPlayer[i]->isUsed == false)
                    {
                        selectedUnit = unitsPlayer[i];
                        selectedUnitOriginalTileX = selectedUnit->tileX;
                        selectedUnitOriginalTileY = selectedUnit->tileY;
                        walkingPath.clear();
                        clearBlueTiles();
                        calculateBlueTiles(selectedUnit);
                        hasTemporarilyWalked = false;
                        clickedOnABlueUnit = true;
                        break;
                    }
                }

                if (!clickedOnABlueUnit)
                {
                    hasSelectedBlankTile = true;
                    menuChoices.clear();
                    menuIdx = 0;
                    menuChoices.push_back("Unit");
                    menuChoices.push_back("Status");
                    menuChoices.push_back("Options");
                    menuChoices.push_back("Suspend");
                    menuChoices.push_back("End");
                }
            }
            else //We have a unit already selected, can we move them to the tile?
            {
                if (hasTemporarilyWalked)
                {
                    executeMenuChoice();
                }
                else if (cursorX == selectedUnit->tileX &&
                         cursorY == selectedUnit->tileY)
                {
                    //skip the walking animation since we are already there.
                    hasTemporarilyWalked = true;
                    calculateMenuChoices();
                }
                else if (tileIsBlue(selectedUnit, cursorX, cursorY))
                {
                    //We also have to check that there isnt another blue unit on this tile already
                    if (getUnitAtTile(cursorX, cursorY, &unitsPlayer) == nullptr)
                    {
                        clearBlueTiles();

                        //generate walking path
                        generateWalkingPath(selectedUnit, cursorX, cursorY);

                        //start walking
                        walkingTimer = (int)walkingPath.size()*8 - 9; //8 frames per tile
                    }
                }
            }
        }
    }

    if (walkingTimer > 0)
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
            direction = 1;
        }
        else if (p2.y < p1.y)
        {
            direction = 2;
        }
        else if (p2.x > p1.x)
        {
            direction = 3;
        }
        else if (p2.x < p1.x)
        {
            direction = 4;
        }

        selectedUnit->x = (int)(tweenX*16);
        selectedUnit->y = (int)(tweenY*16);
        selectedUnit->spriteIndex = direction;
    }

    //Render the blue preview tiles
    if (selectedUnit != nullptr && walkingTimer == 0 && !hasTemporarilyWalked && !hasSelectedBlankTile)
    {
        for (int i = 0; i < previewBlueTiles.size(); i++)
        {
            int tileX = previewBlueTiles[i]->x;
            int tileY = previewBlueTiles[i]->y;
            int pixelX = viewportPixelX + tileX*16;
            int pixelY = viewportPixelY + tileY*16;
            previewBlueTiles[i]->x = pixelX;
            previewBlueTiles[i]->y = pixelY;
            previewBlueTiles[i]->imageIndex++;
            previewBlueTiles[i]->render();
            previewBlueTiles[i]->x = tileX;
            previewBlueTiles[i]->y = tileY;
        }

        if (tileIsBlue(selectedUnit, cursorX, cursorY))
        {
            //Generate walking path, render triangles.
            renderPreviewArrows();
        }
    }

    //Render the red units
    for (int i = 0; i < unitsEnemy.size(); i++)
    {
        unitsEnemy[i]->render(unitsEnemy[i]->tileX*16, unitsEnemy[i]->tileY*16, 5, viewportPixelX, viewportPixelY);
    }

    //Render all of the blue units
    for (int i = 0; i < unitsPlayer.size(); i++)
    {
        if (unitsPlayer[i] == selectedUnit && walkingTimer > 0)
        {
            
        }
        else
        {
            unitsPlayer[i]->render(unitsPlayer[i]->tileX*16, unitsPlayer[i]->tileY*16, 0, viewportPixelX, viewportPixelY);
        }
    }

    if (selectedUnit != nullptr && walkingTimer > 0)
    {
        selectedUnit->render(selectedUnit->x, selectedUnit->y, selectedUnit->spriteIndex, viewportPixelX, viewportPixelY);
    }

    //Render the cursor
    if (!hasTemporarilyWalked && !hasSelectedBlankTile && turnChangeTimer == 0 && !isViewingItems)
    {
        cursor->x = viewportPixelX + cursorX*16;
        cursor->y = viewportPixelY + cursorY*16;
        cursor->imageIndex++;
        cursor->render();
    }

    //Render the hud
    if (selectedUnit == nullptr && !hasSelectedBlankTile && turnChangeTimer == 0 && !isViewingItems)
    {
        renderObjective();
        renderUnitDecription();
        renderTileDescription();
    }

    //Draw options box
    if ((hasTemporarilyWalked || hasSelectedBlankTile) && !isViewingItems)
    {
        int windowBoxX = 16;
        if ((cursorX - viewportX) < 7)
        {
            windowBoxX = 11*16;
        }
        WindowBox::render(3, (int)menuChoices.size() + 1, windowBoxX, 16);
        menuCursor->imageIndex++;
        menuCursor->y = 16 + menuIdx*16;
        menuCursor->x = windowBoxX - 28;
        menuCursor->render();

        for (int i = 0; i < menuChoices.size(); i++)
        {
            Text::renderText(menuChoices[i], Font::White, {255, 255, 255, 255}, windowBoxX + 6, 24 + i*16, false, 0, 0);
        }
    }

    //Draw the item view box
    if (isViewingItems)
    {
        renderItemWindow();

        selectedUnit->sprMugshot->x = 8*16;
        selectedUnit->sprMugshot->y = 16;
        selectedUnit->sprMugshot->imageIndex++;
        selectedUnit->sprMugshot->render();

        WindowBox::render(6, 3, 8*16, 6*16);

        Text::renderText("Affi", Font::White, {255, 255, 255, 255}, 10*16,     6*16 + 3, false, 0, 0);
        Text::renderText("Atk",  Font::White, {255, 255, 255, 255},  8*16 + 6, 7*16,     false, 0, 0);
        Text::renderText("Crit", Font::White, {255, 255, 255, 255}, 11*16    , 7*16,     false, 0, 0);
        Text::renderText("Hit",  Font::White, {255, 255, 255, 255},  8*16 + 6, 8*16 - 2, false, 0, 0);
        Text::renderText("Avd",  Font::White, {255, 255, 255, 255}, 11*16    , 8*16 - 2, false, 0, 0);

        Text::renderText("1",   Font::White, {198, 255, 255, 255},  8*16 + 6 + 17, 7*16,     false, 0, 3);
        Text::renderText("12",  Font::White, {198, 255, 255, 255}, 11*16     + 19, 7*16,     false, 0, 3);
        Text::renderText("123", Font::White, {198, 255, 255, 255},  8*16 + 6 + 17, 8*16 - 2, false, 0, 3);
        Text::renderText("123", Font::White, {198, 255, 255, 255}, 11*16     + 19, 8*16 - 2, false, 0, 3);

        if (itemEditChoices.size() > 0)
        {
            renderItemEditWindow();
        }
    }

    if (turnChangeTimer > 0)
    {
        turnChangeTimer--;

        if (turnChangeTimer == 0)
        {
            isPhaseEnemy  = true;
            isPhasePlayer = false;

            for (int i = 0; i < unitsPlayer.size(); i++)
            {
                unitsPlayer[i]->isUsed = false;
            }
        }
        else
        {
            renderTurnChange(1);
        }
    }
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
        unitsEnemy[i]->render(unitsEnemy[i]->tileX*16, unitsEnemy[i]->tileY*16, 5, viewportPixelX, viewportPixelY);
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
        }
        else
        {
            renderTurnChange(0);
        }
    }
}

void Map::updateCursor()
{
    if (isPhasePlayer && walkingTimer == 0 && !hasTemporarilyWalked && !hasSelectedBlankTile && turnChangeTimer == 0)
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

void Map::clearBlueTiles()
{
    for (int i = 0; i < previewBlueTiles.size(); i++)
    {
        delete previewBlueTiles[i];
    }
    previewBlueTiles.clear();
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

bool Map::tileIsBlue(Unit* unit, int tileX, int tileY)
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

void Map::calculateBlueTiles(Unit* unit)
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

    for (i = 0; i < NUM_NODES; i++)
    {
        if (i != START_NODE)
        {
            //printf("Distance of node %d = %d\n", i, distance[i]);
            if (dijkstraTilesDistance[i] <= unit->mov)
            {
                int nodeX = (i % Map::NODES_WIDTH) - (Map::NODES_WIDTH/2);
                int nodeY = (i / Map::NODES_WIDTH) - (Map::NODES_WIDTH/2);

                int tileX = Map::selectedUnit->tileX + nodeX;
                int tileY = Map::selectedUnit->tileY + nodeY;

                Map::previewBlueTiles.push_back(new Sprite("res/Images/Sprites/Map/PreviewTileBlue", tileX, tileY, false));
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
    }

    //delete[] cost;
    //delete[] graph;
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

    std::vector<Unit*> adjacentEnemyUnits  = getAdjacentUnits(selectedUnit, &unitsEnemy);
    std::vector<Unit*> adjacentPlayerUnits = getAdjacentUnits(selectedUnit, &unitsPlayer);

    if (adjacentEnemyUnits.size() != 0)
    {
        menuChoices.push_back("Attack");
    }

    if (selectedUnit->items.size() > 0)
    {
        menuChoices.push_back("Item");
    }

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
        hasSelectedBlankTile = false;
    }
    else if (menuChoices[menuIdx] == "Attack")
    {
        return;
    }
    else if (menuChoices[menuIdx] == "Trade")
    {
        return;
    }
    else if (menuChoices[menuIdx] == "Item")
    {
        itemIdx = 0;
        isViewingItems = true;

        menuIdx = 0;
        menuChoices.clear();
        return;
    }
    else if (menuChoices[menuIdx] == "Rescue")
    {
        return;
    }
    else if (menuChoices[menuIdx] == "Wait")
    {
        hasTemporarilyWalked = false;
        selectedUnit->isUsed = true;
        selectedUnit = nullptr;
    }

    menuIdx = 0;
    menuChoices.clear();
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

void Map::renderItemWindow()
{
    const int ORIGIN_X = 16;
    const int ORIGIN_Y = 16;

    WindowBox::render(7, (int)selectedUnit->items.size() + 1, ORIGIN_X, ORIGIN_Y);

    if (itemEditChoices.size() == 0)
    {
        menuCursor->imageIndex++;
        menuCursor->y = ORIGIN_X + itemIdx*16;
        menuCursor->x = ORIGIN_Y - 28;
        menuCursor->render();
    }

    for (int i = 0; i < selectedUnit->items.size(); i++)
    {
        Item item = selectedUnit->items[i];
        item.render(ORIGIN_X + 6, ORIGIN_Y + 8 + i*16);
        Text::renderText(item.getName(), Font::White, {255, 255, 255, 255}, ORIGIN_X + 16 + 7, ORIGIN_Y + 8 + i*16, false, 0, 0);
        Text::renderText(std::to_string(item.usesRemaining), Font::White, {198, 255, 255, 255}, ORIGIN_X + 6*16 - 8, ORIGIN_Y + 8 + i*16, false, 0, 2);
    }
}

void Map::renderItemEditWindow()
{
    const int ORIGIN_X = 88;
    const int ORIGIN_Y = 8;

    WindowBox::render(3, (int)itemEditChoices.size() + 1, ORIGIN_X, ORIGIN_Y);
    menuCursor->imageIndex++;
    menuCursor->y = ORIGIN_Y + itemEditIdx*16;
    menuCursor->x = ORIGIN_X - 28;
    menuCursor->render();

    for (int i = 0; i < itemEditChoices.size(); i++)
    {
        Text::renderText(itemEditChoices[i], Font::White, {255, 255, 255, 255}, ORIGIN_X + 8, ORIGIN_Y + 8 + i*16, false, 0, 0);
    }
}
