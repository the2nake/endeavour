#pragma once
#include "Entity.hpp"

#include "pathfinding.hpp"

#include "SDL.h"

#include <vector>

struct Tile
{
    SDL_Texture *texture = nullptr;
    int movementCost = 1;
    bool isNatural = false;
};

class Level
{
public:
    static int tileW, tileH, levelW, levelH;

    static std::vector<Entity *> entities;
    static GridWithWeights pathfindingGrid;

    static void loadPlayerData(std::string playerName, std::string saveName);
    static void loadLevel(std::string playerName, std::string saveName, std::string levelName);
    static void loadNPCs(std::string playerName, std::string saveName, std::string levelName);

    static void renderBackground();
    static void generatePathfindingGrid();

    static GridLocation getTilePosAt(float x, float y)
    {
        return {(int)(std::floor(x / Level::tileW)), (int)(std::floor(y / Level::tileH))};
    }

    static std::string getTileNameAt(float x, float y)
    {
        int gridX = std::floor(x / Level::tileW);
        int gridY = std::floor(y / Level::tileH);
        if (0 <= gridX && gridX < Level::levelW)
        {
            if (0 <= gridY && gridY < Level::levelH)
            {
                return Level::tiles[gridY][gridX];
            }
            else
            {
                return "";
            }
        }
        else
        {
            return "";
        }
    }

    static Tile getTileFromName(std::string name)
    {
        if (tileDataLookup.find(name) != tileDataLookup.end())
        {
            return tileDataLookup[name];
        }
        else
        {
            return Tile{nullptr, 1, false};
        }
    }

    static void clean();

private:
    static std::vector<std::vector<std::string>> tiles;
    static std::unordered_map<std::string, Tile> tileDataLookup; // note that unordered_map does not require the data to have a hash function, only the key
};