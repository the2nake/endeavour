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
    static int tileW;  // in pixels
    static int tileH;  // in pixels
    static int levelW; // in tiles
    static int levelH; // in tiles

    static std::vector<Entity *> entities;
    static GridWithWeights pathfindingGrid;

    static bool loadSave(std::string playerName, std::string saveName);
    static bool loadLevel(std::string playerName, std::string saveName, std::string levelName);
    static bool loadNPCs(std::string playerName, std::string saveName, std::string levelName);

    static void renderBackground();
    static void renderForeground();
    static void generatePathfindingGrid();

    static void clean();

    static GridLocation getPosAt(float x, float y)
    {
        GridLocation tempLoc;
        tempLoc.x = std::floor(x / Level::tileW);
        tempLoc.y = std::floor(y / Level::tileH);
        return tempLoc;
    }

    static std::string getTileNameAtGridPos(std::string layer = "background", int layerNum = 0, GridLocation gridLoc = {0, 0})
    {
        if (0 <= gridLoc.x && gridLoc.x < Level::levelW)
        {
            if (0 <= gridLoc.y && gridLoc.y < Level::levelH)
            {
                if (layer == "foreground")
                {
                    if (Level::foreground.size() < 1)
                        return "";
                    return Level::foreground[layerNum][gridLoc.y][gridLoc.x];
                }
                else
                {
                    if (Level::background.size() < 1)
                        return "";
                    return Level::background[layerNum][gridLoc.y][gridLoc.x];
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

    static std::string getTileNameAtPos(std::string layer = "background", int layerNum = 0, float x = 0, float y = 0)
    {
        int gridX = std::floor(x / Level::tileW);
        int gridY = std::floor(y / Level::tileH);
        return getTileNameAtGridPos(layer, layerNum, GridLocation{gridX, gridY});
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
        return (Level::getTileFromName(Level::getTileNameAtPos("background", 0, x, y)).movementCost +
                Level::getTileFromName(Level::getTileNameAtPos("background", 0, x + w - 1, y)).movementCost +
                Level::getTileFromName(Level::getTileNameAtPos("background", 0, x, y + h - 1)).movementCost +
                Level::getTileFromName(Level::getTileNameAtPos("background", 0, x + w - 1, y + h - 1)).movementCost) /
               4.0f;
    }

    static int getLayerCount(bool checkForeground)
    {
        if (checkForeground)
            return Level::foreground.size();
        else
            return Level::background.size();
    }

private:
    static void renderLayer(std::string layer);
    static std::vector<std::vector<std::vector<std::string>>> background;
    static std::vector<std::vector<std::vector<std::string>>> foreground;
    static std::unordered_map<std::string, Tile> tileDataLookup; // note that unordered_map does not require the data to have a hash function, only the key
    static std::vector<std::string> transparentTiles;
};