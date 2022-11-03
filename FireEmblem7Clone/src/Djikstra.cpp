#include <SDL/SDL.h>

#include <vector>

#include "MapTile.hpp"
#include "Unit.hpp"
#include "Map.hpp"
#include "Sprite.hpp"
#include "Djikstra.hpp"

int*  Djikstra::dijkstraGraph         = nullptr;
int*  Djikstra::dijkstraCost          = nullptr;
int*  Djikstra::dijkstraTilesDistance = nullptr;
int*  Djikstra::dijkstraTilesPath     = nullptr;
bool* Djikstra::dijkstraVisited       = nullptr;

int Djikstra::NUM_NODES = 0;
int Djikstra::NUM_NODES_X = 0;
int Djikstra::NUM_NODES_Y = 0;

void Djikstra::init()
{

}

void Djikstra::allocMapTiles(int numTilesWidth, int numTilesHeight)
{
    if (dijkstraGraph         == nullptr) { delete dijkstraGraph;         }
    if (dijkstraCost          == nullptr) { delete dijkstraCost;          }
    if (dijkstraTilesDistance == nullptr) { delete dijkstraTilesDistance; }
    if (dijkstraTilesPath     == nullptr) { delete dijkstraTilesPath;     }
    if (dijkstraVisited       == nullptr) { delete dijkstraVisited;       }

    NUM_NODES_X = numTilesWidth;
    NUM_NODES_Y = numTilesHeight;
    NUM_NODES = NUM_NODES_X*NUM_NODES_Y;

    dijkstraGraph         = new int [NUM_NODES*NUM_NODES];
    dijkstraCost          = new int [NUM_NODES*NUM_NODES];
    dijkstraTilesDistance = new int [NUM_NODES];
    dijkstraTilesPath     = new int [NUM_NODES];
    dijkstraVisited       = new bool[NUM_NODES];

    memset(dijkstraGraph,         0, sizeof(int) *NUM_NODES*NUM_NODES);
    memset(dijkstraCost,          0, sizeof(int) *NUM_NODES*NUM_NODES);
    memset(dijkstraTilesDistance, 0, sizeof(int) *NUM_NODES);
    memset(dijkstraTilesPath,     0, sizeof(int) *NUM_NODES);
    memset(dijkstraVisited,   false, sizeof(bool)*NUM_NODES);
}

void Djikstra::generateWalkingPath(Unit* unit, int tileX, int tileY, std::vector<SDL_Point>* path, bool removeUnreachableTiles, std::vector<Unit*>* friendlyUnits)
{
    path->clear();
    path->push_back(SDL_Point{tileX, tileY});

    if (unit->tileX == tileX && unit->tileY == tileY)
    {
        return;
    }

    int startNode = unit->tileX + unit->tileY*NUM_NODES_X;
    int endNode = tileX + tileY*NUM_NODES_X;
    int currentNode = endNode;
    do
    {
        currentNode = dijkstraTilesPath[currentNode];

        int thisNodeX = currentNode % NUM_NODES_X;
        int thisNodeY = currentNode / NUM_NODES_X;
        SDL_Point thisTile{thisNodeX, thisNodeY};
        path->push_back(thisTile);
        
    } while (currentNode != startNode);

    if (removeUnreachableTiles)
    {
        // get rid of tiles we cant reach due to movement
        for (int i = (int)path->size() - 1; i >= 0; i--)
        {
            if (!unitCanMoveToTile(unit, path->at(i).x, path->at(i).y))
            {
                path->erase(path->begin(), path->begin() + (i + 1));
                break;
            }
        }

        // get rid of tiles we cant reach because someone else is there
        if (friendlyUnits != nullptr && (int)path->size() > 1)
        {
            while ((int)path->size() > 1)
            {
                bool foundSomeoneAtTheFront = false;
                for (int j = 0; j < (int)friendlyUnits->size(); j++)
                {
                    if (friendlyUnits->at(j) != unit &&
                        friendlyUnits->at(j)->tileX == path->at(0).x &&
                        friendlyUnits->at(j)->tileY == path->at(0).y)
                    {
                        // Trim off the end tile from the path, since we can't reach it if someone is there.
                        path->erase(path->begin());
                        foundSomeoneAtTheFront = true;
                        break;
                    }
                }

                // No one was at the end of the path, so we can reach it now.
                if (!foundSomeoneAtTheFront)
                {
                    break;
                }
            }
        }
    }
}

bool Djikstra::unitCanMoveToTile(Unit* unit, int tileX, int tileY)
{
    if (unit->tileX == tileX && unit->tileY == tileY)
    {
        return true;
    }

    if (tileX < 0 || tileY < 0 || tileX >= NUM_NODES_X || tileY >= NUM_NODES_Y)
    {
        return false;
    }

    return (dijkstraTilesDistance[tileX + tileY*NUM_NODES_X] <= unit->mov);
}

int Djikstra::distanceToTile(Unit* unit, int tileX, int tileY)
{
    if (unit->tileX == tileX && unit->tileY == tileY)
    {
        return 0;
    }

    if (tileX < 0 || tileY < 0 || tileX >= NUM_NODES_X || tileY >= NUM_NODES_Y)
    {
        return 100000000;
    }

    return (dijkstraTilesDistance[tileX + tileY*NUM_NODES_X]);
}

void Djikstra::trimPath(Unit* me, std::vector<SDL_Point>* path, std::vector<Unit*>* unpassableUnits, std::vector<Unit*>* friendlyUnits)
{
    for (int i = (int)path->size() - 1; i >= 0; i--)
    {
        SDL_Point tile = path->at(i);
        for (int j = 0; j < (int)unpassableUnits->size(); j++)
        {
            Unit* unit = unpassableUnits->at(j);
            if (unit->tileX == tile.x &&
                unit->tileY == tile.y)
            {
                path->erase(path->begin(), path->begin() + i + 1);
                i = -1;
                break;
            }
        }
    }

    // make sure that the path doesnt end on a friend
    if (friendlyUnits != nullptr && (int)path->size() > 1)
    {
        while ((int)path->size() > 1)
        {
            bool foundSomeoneAtTheFront = false;
            for (int j = 0; j < (int)friendlyUnits->size(); j++)
            {
                if (friendlyUnits->at(j) != me &&
                    friendlyUnits->at(j)->tileX == path->at(0).x &&
                    friendlyUnits->at(j)->tileY == path->at(0).y)
                {
                    // Trim off the end tile from the path, since we can't reach it if someone is there.
                    path->erase(path->begin());
                    foundSomeoneAtTheFront = true;
                    break;
                }
            }

            // No one was at the end of the path, so we can reach it now.
            if (!foundSomeoneAtTheFront)
            {
                break;
            }
        }
    }
}

void Djikstra::constructGraph(Unit* unit, int* graph, std::vector<Unit*>* unpassableUnits)
{
    for (int node = 0; node < NUM_NODES; node++)
    {
        int currentTileX = node % NUM_NODES_X;
        int currentTileY = node / NUM_NODES_X;

        MapTile tileUp    = Map::getTile(currentTileX,     currentTileY - 1, unpassableUnits);
        MapTile tileDown  = Map::getTile(currentTileX,     currentTileY + 1, unpassableUnits);
        MapTile tileLeft  = Map::getTile(currentTileX - 1, currentTileY    , unpassableUnits);
        MapTile tileRight = Map::getTile(currentTileX + 1, currentTileY    , unpassableUnits);

        int nodeUp    = node - NUM_NODES_X;
        int nodeDown  = node + NUM_NODES_X;
        int nodeLeft  = node - 1;
        int nodeRight = node + 1;

        bool upIsValid    = ((node - NUM_NODES_X) >= 0);
        bool downIsValid  = ((node + NUM_NODES_X) <  NUM_NODES);
        bool leftIsValid  = ((node % NUM_NODES_X) != 0);
        bool rightIsValid = ((node % NUM_NODES_X) != NUM_NODES_X - 1);

        if (upIsValid   ) { graph[node*NUM_NODES + nodeUp   ] = tileUp   .movementLostForMovingOnThisTile(unit); }
        if (downIsValid ) { graph[node*NUM_NODES + nodeDown ] = tileDown .movementLostForMovingOnThisTile(unit); }
        if (leftIsValid ) { graph[node*NUM_NODES + nodeLeft ] = tileLeft .movementLostForMovingOnThisTile(unit); }
        if (rightIsValid) { graph[node*NUM_NODES + nodeRight] = tileRight.movementLostForMovingOnThisTile(unit); }
    }
}

void Djikstra::calculatePreviewTiles(Unit* unit, std::vector<Sprite*>* blueTiles, std::vector<Sprite*>* redTiles, std::vector<Unit*>* friendlyUnits, std::vector<Unit*>* unpassableUnits)
{
    memset(dijkstraGraph,         0, sizeof(int) *NUM_NODES*NUM_NODES);
    memset(dijkstraCost,          0, sizeof(int) *NUM_NODES*NUM_NODES);
    memset(dijkstraTilesDistance, 0, sizeof(int) *NUM_NODES);
    //memset(dijkstraTilesPath,     0, sizeof(int) *NUM_NODES);
    //memset(dijkstraVisited,       0, sizeof(bool)*NUM_NODES);

    constructGraph(unit, dijkstraGraph, unpassableUnits);

    const int INF = 100000000;

    int count, mindistance, nextnode = 0, i, j;
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

    const int START_NODE = unit->tileX + unit->tileY*NUM_NODES_X;
    for (i = 0; i < NUM_NODES; i++)
    {
        dijkstraTilesDistance[i] = dijkstraCost[START_NODE*NUM_NODES + i];
        dijkstraTilesPath[i] = START_NODE;
        dijkstraVisited[i] = false;
    }
    dijkstraTilesDistance[START_NODE] = 0;
    dijkstraVisited[START_NODE] = true;
    count = 1;
    while (count < NUM_NODES - 1)
    {
        mindistance = INF;
        for (i = 0; i < NUM_NODES; i++)
        {
            if (dijkstraTilesDistance[i] < mindistance && !dijkstraVisited[i])
            {
                mindistance = dijkstraTilesDistance[i];
                nextnode = i;
            }
        }
        dijkstraVisited[nextnode] = true;
        for (i = 0; i < NUM_NODES; i++)
        {
            if (!dijkstraVisited[i])
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

    std::unordered_set<int> blueTilesSet;

    for (i = 0; i < NUM_NODES; i++)
    {
        if (dijkstraTilesDistance[i] <= unit->mov)
        {
            int tileX = (i % NUM_NODES_X);
            int tileY = (i / NUM_NODES_X);

            blueTiles->push_back(new Sprite("res/Images/Sprites/Map/PreviewTileBlue", tileX, tileY, false));
            blueTilesSet.insert(tileX | (tileY << 16));
        }

        j = i;
        do
        {
            j = dijkstraTilesPath[j];
        } while (j != START_NODE);
    }

    std::unordered_set<int> attackRanges = unit->getAttackRanges();
    std::unordered_set<int> redTilesSet;

    for (int t = 0; t < blueTiles->size(); t++)
    {
        int baseX = blueTiles->at(t)->x;
        int baseY = blueTiles->at(t)->y;

        Unit* friendAtTile = Map::getUnitAtTile(baseX, baseY, friendlyUnits);
        Unit* foeAtTile    = Map::getUnitAtTile(baseX, baseY, unpassableUnits);

        if ((friendAtTile != unit && friendAtTile != nullptr) || foeAtTile != nullptr)
        {
            continue;
        }

        std::vector<SDL_Point> newReds = Map::calculatePreviewTilesAtTile(baseX, baseY, attackRanges);

        for (SDL_Point p : newReds)
        {
            redTilesSet.insert(p.x | (p.y << 16));
        }
    }

    for (auto itr = redTilesSet.begin(); itr != redTilesSet.end(); ++itr)
    {
        int newRedTile = *itr;

        auto contains = blueTilesSet.find(newRedTile);
        if (contains == blueTilesSet.end()) //Dont create if already a blue tile
        {
            int x = newRedTile & 0xFFFF;
            int y = (newRedTile >> 16) & 0xFFFF;
            redTiles->push_back(new Sprite("res/Images/Sprites/Map/PreviewTileRed", x, y, false));
        }
    }
}

void Djikstra::calculateBlueTiles(Unit* unit, std::vector<SDL_Point>* blueTiles, std::vector<Unit*>* friendlyUnits, std::vector<Unit*>* unpassableUnits)
{
    memset(dijkstraGraph,         0, sizeof(int)*NUM_NODES*NUM_NODES);
    memset(dijkstraCost,          0, sizeof(int)*NUM_NODES*NUM_NODES);
    memset(dijkstraTilesDistance, 0, sizeof(int)*NUM_NODES);
    //memset(dijkstraTilesPath,     0, sizeof(int)*NUM_NODES);
    //memset(dijkstraVisited,       0, sizeof(int)*NUM_NODES); //might not actually need to memset this. also this can be a bool

    constructGraph(unit, dijkstraGraph, unpassableUnits);

    const int INF = 100000000;

    int count, mindistance, nextnode = 0, i, j;
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

    const int START_NODE = unit->tileX + unit->tileY*NUM_NODES_X;
    for (i = 0; i < NUM_NODES; i++)
    {
        dijkstraTilesDistance[i] = dijkstraCost[START_NODE*NUM_NODES + i];
        dijkstraTilesPath[i] = START_NODE;
        dijkstraVisited[i] = false;
    }
    dijkstraTilesDistance[START_NODE] = 0;
    dijkstraVisited[START_NODE] = true;
    count = 1;
    while (count < NUM_NODES - 1)
    {
        mindistance = INF;
        for (i = 0; i < NUM_NODES; i++)
        {
            if (dijkstraTilesDistance[i] < mindistance && !dijkstraVisited[i])
            {
                mindistance = dijkstraTilesDistance[i];
                nextnode = i;
            }
        }
        dijkstraVisited[nextnode] = true;
        for (i = 0; i < NUM_NODES; i++)
        {
            if (!dijkstraVisited[i])
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
        if (dijkstraTilesDistance[i] <= unit->mov)
        {
            int tileX = (i % NUM_NODES_X);
            int tileY = (i / NUM_NODES_X);

            bool unitAlreadyThere = false;

            for (int f = 0; f < friendlyUnits->size(); f++)
            {
                Unit* friendlyUnit = friendlyUnits->at(f);
                if (friendlyUnit->tileX == tileX &&
                    friendlyUnit->tileY == tileY &&
                    friendlyUnit != unit)
                {
                    unitAlreadyThere = true;
                    break;
                }
            }

            if (!unitAlreadyThere)
            {
                blueTiles->push_back(SDL_Point{tileX, tileY});
            }
        }

        j = i;
        do
        {
            j = dijkstraTilesPath[j];
        } while (j != START_NODE);
    }
}

void Djikstra::calculateRedTiles(Unit* unit, std::vector<SDL_Point>* redTiles, std::vector<Unit*>* friendlyUnits, std::vector<Unit*>* unpassableUnits)
{
    std::vector<SDL_Point> blueTiles;
    calculateBlueTiles(unit, &blueTiles, friendlyUnits, unpassableUnits);

    std::unordered_set<int> attackRanges = unit->getAttackRanges();
    std::unordered_set<int> redTilesSet;

    for (int t = 0; t < blueTiles.size(); t++)
    {
        int baseX = blueTiles[t].x;
        int baseY = blueTiles[t].y;

        Unit* friendAtTile = Map::getUnitAtTile(baseX, baseY, friendlyUnits);
        Unit* foeAtTile    = Map::getUnitAtTile(baseX, baseY, unpassableUnits);

        if ((friendAtTile != unit && friendAtTile != nullptr) || foeAtTile != nullptr)
        {
            continue;
        }

        std::vector<SDL_Point> newReds = Map::calculatePreviewTilesAtTile(baseX, baseY, attackRanges);

        for (SDL_Point p : newReds)
        {
            redTilesSet.insert(p.x | (p.y << 16));
        }
    }

    for (auto itr = redTilesSet.begin(); itr != redTilesSet.end(); ++itr)
    {
        int newRedTile = *itr;

        int x = newRedTile & 0xFFFF;
        int y = (newRedTile >> 16) & 0xFFFF;
        redTiles->push_back({x, y});
    }
}
