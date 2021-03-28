#include <SDL/SDL.h>

#include <vector>

#include "MapTile.hpp"
#include "Unit.hpp"
#include "Map.hpp"
#include "Sprite.hpp"
#include "Djikstra.hpp"

int* Djikstra::dijkstraGraph         = nullptr;
int* Djikstra::dijkstraCost          = nullptr;
int* Djikstra::dijkstraTilesDistance = nullptr;
int* Djikstra::dijkstraTilesPath     = nullptr;

void Djikstra::init()
{
    //too big for the stack
    dijkstraGraph         = new int[NUM_NODES*NUM_NODES];
    dijkstraCost          = new int[NUM_NODES*NUM_NODES];
    dijkstraTilesDistance = new int[NUM_NODES];
    dijkstraTilesPath     = new int[NUM_NODES];

    memset(dijkstraGraph,         0, sizeof(int)*NUM_NODES*NUM_NODES);
    memset(dijkstraCost,          0, sizeof(int)*NUM_NODES*NUM_NODES);
    memset(dijkstraTilesDistance, 0, sizeof(int)*NUM_NODES);
    memset(dijkstraTilesPath,     0, sizeof(int)*NUM_NODES);
}

void Djikstra::generateWalkingPath(Unit* unit, int tileX, int tileY, std::vector<SDL_Point>* path)
{
    path->clear();
    path->push_back(SDL_Point{tileX, tileY});

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
        path->push_back(thisTile);
        
    } while(currentNode != NUM_NODES/2);
}

bool Djikstra::unitCanMoveToTile(Unit* unit, int tileX, int tileY)
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

void Djikstra::constructGraph(Unit* unit, int* graph, std::vector<Unit*>* unpassableUnits)
{
    int startTileX = unit->tileX - MAX_MOVEMENT;
    int startTileY = unit->tileY - MAX_MOVEMENT;

    for (int node = 0; node < NUM_NODES; node++)
    {
        int currentTileX = node % NODES_WIDTH;
        int currentTileY = node / NODES_WIDTH;

        MapTile tileUp    = Map::getTile(startTileX + currentTileX,     startTileY + currentTileY - 1, unpassableUnits);
        MapTile tileDown  = Map::getTile(startTileX + currentTileX,     startTileY + currentTileY + 1, unpassableUnits);
        MapTile tileLeft  = Map::getTile(startTileX + currentTileX - 1, startTileY + currentTileY    , unpassableUnits);
        MapTile tileRight = Map::getTile(startTileX + currentTileX + 1, startTileY + currentTileY    , unpassableUnits);

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

void Djikstra::calculatePreviewTiles(Unit* unit, std::vector<Sprite*>* blueTiles, std::vector<Sprite*>* redTiles, std::vector<Unit*>* friendlyUnits, std::vector<Unit*>* unpassableUnits)
{
    memset(dijkstraGraph,         0, sizeof(int)*NUM_NODES*NUM_NODES);
    memset(dijkstraCost,          0, sizeof(int)*NUM_NODES*NUM_NODES);
    memset(dijkstraTilesDistance, 0, sizeof(int)*NUM_NODES);
    memset(dijkstraTilesPath,     0, sizeof(int)*NUM_NODES);

    constructGraph(unit, dijkstraGraph, unpassableUnits);

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

    std::unordered_set<int> blueTilesSet;

    for (i = 0; i < NUM_NODES; i++)
    {
        if (dijkstraTilesDistance[i] <= unit->mov)
        {
            int nodeX = (i % NODES_WIDTH) - (NODES_WIDTH/2);
            int nodeY = (i / NODES_WIDTH) - (NODES_WIDTH/2);

            int tileX = unit->tileX + nodeX;
            int tileY = unit->tileY + nodeY;

            blueTiles->push_back(new Sprite("res/Images/Sprites/Map/PreviewTileBlue", tileX, tileY, false));
            blueTilesSet.insert(tileX | (tileY << 16));
        }

        j = i;
        do
        {
            j = dijkstraTilesPath[j];
        } while(j != START_NODE);
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

        std::unordered_set<int> newReds = Map::calculateRedTilesAtTile(baseX, baseY, attackRanges);
        redTilesSet.insert(newReds.begin(), newReds.end());
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
