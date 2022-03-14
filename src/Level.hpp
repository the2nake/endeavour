#pragma once
#include "Entity.hpp"

#include <vector>
#include "SDL.h"
#include "pathfinding.hpp"


struct Tile {
    SDL_Texture* texture;
    bool traversable;
};

class Level
{
public:
    static std::vector<Entity *> entities;
    static GridWithWeights pathfindingGrid;

    static std::vector<std::vector<std::string>> tiles;
    static std::unordered_map<std::string, Tile> tileDataLookup;

    static void loadPlayerData(std::string playerName, std::string saveName);
    static void loadLevel(std::string pathToLevelData);
};