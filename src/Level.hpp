#pragma once
#include "Entity.hpp"

#include "pathfinding.hpp"

#include "SDL.h"

#include <vector>

struct Tile
{
    SDL_Texture *texture = nullptr;
    float movementCost = 1.0f;
    bool isNatural = false;
    SDL_Rect collisionRect1;
    SDL_Rect collisionRect2;
};

class Level
{
public:
    static int tileW, tileH;   // in pixels
    static int levelW, levelH; // in tiles

    static std::vector<Entity *> entities;
    static GridWithWeights pathfindingGrid;

    static void loadPlayerData(std::string playerName, std::string saveName);
    static void loadLevel(std::string playerName, std::string saveName, std::string levelName);
    static void loadNPCs(std::string playerName, std::string saveName, std::string levelName);

    static void renderBackground();
    static void renderForeground();
    static void generatePathfindingGrid();

    static void clean();

    static GridLocation getTilePosAt(float x, float y)
    {
        GridLocation tempLoc;
        tempLoc.x = std::floor(x / Level::tileW);
        tempLoc.y = std::floor(y / Level::tileH);
        return tempLoc;
    }

    static std::string getTileNameAtPosition(std::string layer, int layerNum, float x, float y)
    {
        int gridX = std::floor(x / Level::tileW);
        int gridY = std::floor(y / Level::tileH);
        if (0 <= gridX && gridX < Level::levelW)
        {
            if (0 <= gridY && gridY < Level::levelH)
            {
                if (layer == "foreground")
                {
                    return Level::foreground[layerNum][gridY][gridX];
                }
                else
                {
                    return Level::background[layerNum][gridY][gridX];
                }
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

    static float getMovementCostInArea(float x, float y, int w, int h)
    {
        // INFO: only the bottom background layer counts when using movementCost
        return (Level::getTileFromName(Level::getTileNameAtPosition("background", 0, x, y)).movementCost +
                Level::getTileFromName(Level::getTileNameAtPosition("background", 0, x + w - 1, y)).movementCost +
                Level::getTileFromName(Level::getTileNameAtPosition("background", 0, x, y + h - 1)).movementCost +
                Level::getTileFromName(Level::getTileNameAtPosition("background", 0, x + w - 1, y + h - 1)).movementCost) /
               4.0f;
    }

    static int getLayerCount(bool checkForeground) {
        if (checkForeground) return Level::foreground.size();
        else return Level::background.size();
    }

private:
    static void renderLayer(std::string layer);
    static std::vector<std::vector<std::vector<std::string>>> background;
    static std::vector<std::vector<std::vector<std::string>>> foreground;
    static std::unordered_map<std::string, Tile> tileDataLookup; // note that unordered_map does not require the data to have a hash function, only the key
};