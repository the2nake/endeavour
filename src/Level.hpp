#pragma once
#include "Entity.hpp"

#include <vector>
#include "SDL.h"
#include "pathfinding.hpp"


struct Tile {
    SDL_Texture* texture;
    int movementCost;
};

class Level
{
public:
    static int gridW, gridH;

    static std::vector<Entity *> entities;
    static GridWithWeights pathfindingGrid;

    static void loadPlayerData(std::string playerName, std::string saveName);
    static void loadLevel(std::string playerName, std::string saveName, std::string levelName);

    static void renderBackground();
private:
    static std::vector<std::vector<std::string>> tiles;
    static std::unordered_map<std::string, Tile> tileDataLookup; // note that unordered_map does not require the data to have a hash function, only the key
};