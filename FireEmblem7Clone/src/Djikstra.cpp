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
int* Djikstra::dijkstraVisited       = nullptr;

int Djikstra::NUM_NODES = 0;
int Djikstra::NUM_NODES_X = 0;
int Djikstra::NUM_NODES_Y = 0;

void Djikstra::init()
{
    //too big for the stack
    //dijkstraGraph         = new int[NUM_NODES*NUM_NODES];
    //dijkstraCost          = new int[NUM_NODES*NUM_NODES];
    //dijkstraTilesDistance = new int[NUM_NODES];
    //dijkstraTilesPath     = new int[NUM_NODES];
    //
    //memset(dijkstraGraph,         0, sizeof(int)*NUM_NODES*NUM_NODES);
    //memset(dijkstraCost,          0, sizeof(int)*NUM_NODES*NUM_NODES);
    //memset(dijkstraTilesDistance, 0, sizeof(int)*NUM_NODES);
    //memset(dijkstraTilesPath,     0, sizeof(int)*NUM_NODES);
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

    dijkstraGraph         = new int[NUM_NODES*NUM_NODES];
    dijkstraCost          = new int[NUM_NODES*NUM_NODES];
    dijkstraTilesDistance = new int[NUM_NODES];
    dijkstraTilesPath     = new int[NUM_NODES];
    dijkstraVisited       = new int[NUM_NODES];

    memset(dijkstraGraph,         0, sizeof(int)*NUM_NODES*NUM_NODES);
    memset(dijkstraCost,          0, sizeof(int)*NUM_NODES*NUM_NODES);
    memset(dijkstraTilesDistance, 0, sizeof(int)*NUM_NODES);
    memset(dijkstraTilesPath,     0, sizeof(int)*NUM_NODES);
    memset(dijkstraVisited,       0, sizeof(int)*NUM_NODES);
}

void Djikstra::generateWalkingPath(Unit* unit, int tileX, int tileY, std::vector<SDL_Point>* path)
{
    path->clear();
    path->push_back(SDL_Point{tileX, tileY});

    if (unit->tileX == tileX && unit->tileY == tileY)
    {
        return;
    }

    //int nodeX = (tileX - unit->tileX) + MAX_MOVEMENT;
    //int nodeY = (tileY - unit->tileY) + MAX_MOVEMENT;
    int startNode = unit->tileX + unit->tileY*NUM_NODES_X;
    int endNode = tileX + tileY*NUM_NODES_X;//nodeX + nodeY*NODES_WIDTH;
    int currentNode = endNode;
    do
    {
        currentNode = dijkstraTilesPath[currentNode];

        int thisNodeX = currentNode % NUM_NODES_X;//(currentNode % NODES_WIDTH) - MAX_MOVEMENT;
        int thisNodeY = currentNode / NUM_NODES_X;//(currentNode / NODES_WIDTH) - MAX_MOVEMENT;
        //SDL_Point thisTile {unit->tileX + thisNodeX, unit->tileY + thisNodeY};
        SDL_Point thisTile{thisNodeX, thisNodeY};
        path->push_back(thisTile);
        
    } while (currentNode != startNode); //NUM_NODES/2
}

bool Djikstra::unitCanMoveToTile(Unit* unit, int tileX, int tileY)
{
    if (unit->tileX == tileX && unit->tileY == tileY)
    {
        return true;
    }

    int nodeX = tileX;//(tileX - unit->tileX) + MAX_MOVEMENT;
    int nodeY = tileY;//(tileY - unit->tileY) + MAX_MOVEMENT;

    if (nodeX < 0 || nodeY < 0 || nodeX >= NUM_NODES_X || nodeY >= NUM_NODES_Y) //NODES_WIDTH NODES_WIDTH
    {
        return false;
    }

    return (dijkstraTilesDistance[nodeX + nodeY*NUM_NODES_X] <= unit->mov); //NODES_WIDTH
}

void Djikstra::constructGraph(Unit* unit, int* graph, std::vector<Unit*>* unpassableUnits)
{
    int startTileX = 0;//unit->tileX;// - MAX_MOVEMENT;
    int startTileY = 0;//unit->tileY;// - MAX_MOVEMENT;

    for (int node = 0; node < NUM_NODES; node++)
    {
        int currentTileX = node % NUM_NODES_X;//NODES_WIDTH;
        int currentTileY = node / NUM_NODES_X;//NODES_WIDTH;

        MapTile tileUp    = Map::getTile(startTileX + currentTileX,     startTileY + currentTileY - 1, unpassableUnits);
        MapTile tileDown  = Map::getTile(startTileX + currentTileX,     startTileY + currentTileY + 1, unpassableUnits);
        MapTile tileLeft  = Map::getTile(startTileX + currentTileX - 1, startTileY + currentTileY    , unpassableUnits);
        MapTile tileRight = Map::getTile(startTileX + currentTileX + 1, startTileY + currentTileY    , unpassableUnits);

        int nodeUp    = node - NUM_NODES_X;//NODES_WIDTH;
        int nodeDown  = node + NUM_NODES_X;//NODES_WIDTH;
        int nodeLeft  = node - 1;
        int nodeRight = node + 1;

        bool upIsValid    = ((node - NUM_NODES_X) >= 0);            //NODES_WIDTH
        bool downIsValid  = ((node + NUM_NODES_X) <  NUM_NODES);    //NODES_WIDTH
        bool leftIsValid  = ((node % NUM_NODES_X) != 0);            //NODES_WIDTH
        bool rightIsValid = ((node % NUM_NODES_X) != NUM_NODES_X - 1); //NODES_WIDTH, NODES_WIDTH

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
    memset(dijkstraVisited,       0, sizeof(int)*NUM_NODES); //might not actually need to memset this. also this can be a bool

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

    const int START_NODE = unit->tileX + unit->tileY*NUM_NODES_X;//NUM_NODES/2;
    for (i = 0; i < NUM_NODES; i++)
    {
        dijkstraTilesDistance[i] = dijkstraCost[START_NODE*NUM_NODES + i];
        dijkstraTilesPath[i] = START_NODE;
        dijkstraVisited[i] = 0;
    }
    dijkstraTilesDistance[START_NODE] = 0;
    dijkstraVisited[START_NODE] = 1;
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
        dijkstraVisited[nextnode] = 1;
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
    //std::vector<SDL_Point> blueTilesSet;

    for (i = 0; i < NUM_NODES; i++)
    {
        if (dijkstraTilesDistance[i] <= unit->mov)
        {
            int nodeX = (i % NUM_NODES_X);// - (NODES_WIDTH/2); //NODES_WIDTH NODES_WIDTH/2
            int nodeY = (i / NUM_NODES_X);// - (NODES_WIDTH/2); //NODES_WIDTH NODES_WIDTH/2

            int tileX = nodeX;//unit->tileX + nodeX;
            int tileY = nodeY;//unit->tileY + nodeY;

            blueTiles->push_back(new Sprite("res/Images/Sprites/Map/PreviewTileBlue", tileX, tileY, false));
            blueTilesSet.insert(tileX | (tileY << 16));
            //blueTilesSet.push_back(SDL_Point{tileX, tileY});
        }

        j = i;
        do
        {
            j = dijkstraTilesPath[j];
        } while (j != START_NODE);
    }

    std::unordered_set<int> attackRanges = unit->getAttackRanges();
    std::unordered_set<int> redTilesSet;
    //std::vector<SDL_Point> redTilesSet;

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
        //redTilesSet.insert(newReds.begin(), newReds.end());
        for (SDL_Point p : newReds)
        {
            redTilesSet.insert(p.x | (p.y << 16));
        }
    }

    for (auto itr = redTilesSet.begin(); itr != redTilesSet.end(); ++itr)
    {
        int newRedTile = *itr;

        //bool contains = false;
        //for (SDL_Point p : blueTilesSet)
        //{
        //    if (p.x == newRedTile.x &&
        //        p.y == newRedTile.y)
        //    {
        //        contains = true;
        //        break;
        //    }
        //}
        //
        //if (!contains)
        //{
        //    redTiles->push_back(new Sprite("res/Images/Sprites/Map/PreviewTileRed", newRedTile.x, newRedTile.y, false));
        //}
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
    memset(dijkstraTilesPath,     0, sizeof(int)*NUM_NODES);
    memset(dijkstraVisited,       0, sizeof(int)*NUM_NODES); //might not actually need to memset this. also this can be a bool

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

    const int START_NODE = unit->tileX + unit->tileY*NUM_NODES_X;//NUM_NODES/2;
    for (i = 0; i < NUM_NODES; i++)
    {
        dijkstraTilesDistance[i] = dijkstraCost[START_NODE*NUM_NODES + i];
        dijkstraTilesPath[i] = START_NODE;
        dijkstraVisited[i] = 0;
    }
    dijkstraTilesDistance[START_NODE] = 0;
    dijkstraVisited[START_NODE] = 1;
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
        dijkstraVisited[nextnode] = 1;
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
            int nodeX = (i % NUM_NODES_X);// - (NODES_WIDTH/2); //NODES_WIDTH NODES_WIDTH/2
            int nodeY = (i / NUM_NODES_X);// - (NODES_WIDTH/2); //NODES_WIDTH NODES_WIDTH/2

            int tileX = nodeX;//unit->tileX + nodeX;
            int tileY = nodeY;//unit->tileY + nodeY;

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
