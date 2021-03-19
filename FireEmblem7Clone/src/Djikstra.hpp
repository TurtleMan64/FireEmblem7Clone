#pragma once

class Unit;

class Djikstra
{
private:
    static const int MAX_MOVEMENT = 13;
    static const int NODES_WIDTH = MAX_MOVEMENT*2 + 1;
    static const int NUM_NODES = NODES_WIDTH*NODES_WIDTH;

    static int* dijkstraGraph;         //Here so that it doesnt need to malloc every time
    static int* dijkstraCost;          //Here so that it doesnt need to malloc every time
    static int* dijkstraTilesDistance; //Distance from unit to each tile
    static int* dijkstraTilesPath;     //Path from a tile to previous tile on optimal path
    static void constructGraph(Unit* unit, int* graph, std::vector<Unit*>* unpassableUnits);

public:
    //Creates the blue and red preview tiles for a unit
    static void calculatePreviewTiles(Unit* unit, std::vector<Sprite*>* blueTiles, std::vector<Sprite*>* redTiles, std::vector<Unit*>* friendlyUnits, std::vector<Unit*>* unpassableUnits);
    //Creates the path a unit will take to a tile
    static void generateWalkingPath(Unit* unit, int tileX, int tileY, std::vector<SDL_Point>* path);
    static bool unitCanMoveToTile(Unit* unit, int tileX, int tileY);
};
