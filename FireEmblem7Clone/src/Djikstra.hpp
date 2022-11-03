#pragma once

#include <SDL/SDL.h>

#include <vector>

class Unit;
class Sprite;

class Djikstra
{
private:
    static int NUM_NODES_X;
    static int NUM_NODES_Y;
    static int NUM_NODES;

    static int*  dijkstraGraph;         //Here so that it doesnt need to malloc every time
    static int*  dijkstraCost;          //Here so that it doesnt need to malloc every time
    static int*  dijkstraTilesDistance; //Distance from unit to each tile
    static int*  dijkstraTilesPath;     //Path from a tile to previous tile on optimal path
    static bool* dijkstraVisited;
    static void constructGraph(Unit* unit, int* graph, std::vector<Unit*>* unpassableUnits);

public:
    static void init();
    static void allocMapTiles(int numTilesWidth, int numTilesHeight);
    // Creates the blue and red preview tiles for a unit
    static void calculatePreviewTiles(Unit* unit, std::vector<Sprite*>* blueTiles, std::vector<Sprite*>* redTiles, std::vector<Unit*>* friendlyUnits, std::vector<Unit*>* unpassableUnits);
    // Creates tiles that the unit can STAND on. this means it wont include spots that other friendly units are standing on already.
    static void calculateBlueTiles(Unit* unit, std::vector<SDL_Point>* blueTiles, std::vector<Unit*>* friendlyUnits, std::vector<Unit*>* unpassableUnits);
    // Creates tiles that the unit can attack. Used for the attack preview tiles for all enemies combined.
    static void calculateRedTiles(Unit* unit, std::vector<SDL_Point>* blueTiles, std::vector<Unit*>* friendlyUnits, std::vector<Unit*>* unpassableUnits);
    // Creates a shortest path a unit will take to a tile. removeUnreachableTiles will remove the tiles that the unit can't reach. Only call this after calculatePreviewTiles or calculateBlueTiles is called as it re-uses the stuff from those.
    static void generateWalkingPath(Unit* unit, int tileX, int tileY, std::vector<SDL_Point>* path, bool removeUnreachableTiles = false, std::vector<Unit*>* friendlyUnits = nullptr);
    static bool unitCanMoveToTile(Unit* unit, int tileX, int tileY);
    static void trimPath(Unit* me, std::vector<SDL_Point>* path, std::vector<Unit*>* unpassableUnits, std::vector<Unit*>* friendlyUnits);
    // How much movement it takes to get to a tile
    static int distanceToTile(Unit* unit, int tileX, int tileY);
};
