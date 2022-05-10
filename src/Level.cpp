#include "Level.hpp"

#include "Entity.hpp"
#include "AI.hpp"
#include "Game.hpp"
#include "TextureManager.hpp"
#include "common.hpp"

#include "pugixml.hpp"
#include "SDL.h"

#include <iostream>
#include <filesystem>

int Level::tileW = 0;
int Level::tileH = 0;
int Level::levelW = 0;
int Level::levelH = 0;

GridWithWeights Level::pathfindingGrid{0, 0};

std::vector<Entity *> Level::entities;
std::unordered_map<std::string, Tile> Level::tileDataLookup;
std::vector<std::vector<std::string>> Level::tiles;

void Level::loadPlayerData(std::string playerName, std::string saveName)
{
    using namespace pugi;

    std::string pathToSave = "saves/" + playerName + "/" + saveName + "/player.xml";
    if (std::filesystem::exists(pathToSave.c_str()))
    {
        xml_document playerFile;
        xml_parse_result result = playerFile.load_file(pathToSave.c_str());

        if (result)
        {
            xml_node playerNode = playerFile.child("player");

            std::string loadedPlayerName = playerNode.attribute("name").as_string();
            int playerX = playerNode.attribute("x").as_float();
            int playerY = playerNode.attribute("y").as_float();
            xml_node firstFrameNode = playerNode.child("animation").child("frame");

            SDL_Rect cropRect = stringToSDLRect(firstFrameNode.attribute("crop").as_string());
            SDL_Rect outRect = stringToSDLRect(firstFrameNode.attribute("outRect").as_string());
            SDL_Texture *playerTexture = TextureManager::loadTexture(firstFrameNode.attribute("texture").as_string(), &cropRect, &outRect);

            Game::player = Player();
            Game::player.init(playerX, playerY, loadedPlayerName, playerTexture);
            Game::player.setAttribute("speed", playerNode.attribute("speed").as_string() == "" ? (float)(1) : playerNode.attribute("speed").as_float());
        }
        else
        {
            // problem loading save
        }
    }
    else
    {
        std::cout << "Save file at " + pathToSave + " could not be found." << std::endl;
    }
}

void Level::loadLevel(std::string playerName, std::string saveName, std::string levelName)
{
    using namespace pugi;

    xml_document saveFile;
    std::string pathToSave = "saves/" + playerName + "/" + saveName + "/levels/" + levelName + ".xml";
    xml_parse_result result = saveFile.load_file(pathToSave.c_str());
    xml_node levelEl, tileIndexEl, backgroundEl;
    Tile tempTile;
    if (result)
    {
        Level::tiles.clear();

        tileIndexEl = saveFile.child("tiles");

        levelEl = saveFile.child("level");
        backgroundEl = levelEl.child("background");

        Level::tileW = levelEl.attribute("tileW").as_int();
        Level::tileH = levelEl.attribute("tileH").as_int();

        for (xml_node tileEl : tileIndexEl.children("tile"))
        {
            SDL_Rect src = stringToSDLRect(tileEl.attribute("cropRect").as_string());
            SDL_Rect dst = stringToSDLRect(tileEl.attribute("outRect").as_string());
            tempTile.texture = TextureManager::loadTexture(tileEl.attribute("src").as_string(), &src, &dst); // this should point to the same texture pointed to in TextureManager::loadedTextures
                                                                                                             // if there are duplcicates
            tempTile.movementCost = tileEl.attribute("movementCost").as_int(1);
            tempTile.isNatural = tileEl.attribute("isNatural").as_bool();
            Level::tileDataLookup.insert_or_assign(tileEl.attribute("name").as_string(), tempTile);
        }

        int rowNum = 0;
        for (xml_node row : backgroundEl.children("row"))
        {
            tiles.push_back({});
            std::string rowText = row.text().as_string();
            std::vector<std::string> rowSplit = {};
            splitString(rowSplit, rowText, ",");
            Level::levelW = 0;
            for (std::string tileName : rowSplit)
            {
                Level::tiles[rowNum].push_back(trimWhitespace(tileName));
                Level::levelW += 1;
            }
            rowNum++;
        }
        Level::levelH = rowNum;
        // TODO: create a graph for A*
        Level::loadNPCs(playerName, saveName, levelName);
        Level::generatePathfindingGrid();
    }
    else
    {
        // problem loading save
        // check if file exists
        // check if file has proper syntax (single tags need to have </>, etc.)
    }
}

void Level::loadNPCs(std::string playerName, std::string saveName, std::string levelName)
{
    using namespace pugi;

    xml_document npcFile, levelFile;
    std::string npcFileFolder = "saves/" + playerName + "/" + saveName + "/npcs/";
    std::string levelFilePath = "saves/" + playerName + "/" + saveName + "/levels/" + levelName + ".xml";
    xml_parse_result result = levelFile.load_file(levelFilePath.c_str());

    std::vector<std::string> npcsToLoad = {};
    for (xml_node npcListingEl : levelFile.child("npcs").children("npc"))
    {
        npcsToLoad.push_back(npcListingEl.attribute("id-name").as_string());
    }

    for (std::string npcName : npcsToLoad)
    {
        std::string pathToSave = npcFileFolder + npcName + ".xml";
        xml_parse_result result = npcFile.load_file(pathToSave.c_str());
        // Valid npc files are: adjacent to this level, have proper format, and contain at least one npc
        xml_node npcEl = npcFile.child("npc");
        // TODO: write unit test for level loading functions
        // TODO: Make load surrounding levels (low-priority, requires level exits)
        xml_node textureEl = npcEl.child("animation").child("frame");
        if (textureEl.attribute("src").as_string() != "")
        {
            SDL_Rect *cropRect, *outDim;
            SDL_Rect tmpCrop, tmpOut;
            std::string pathToNPCTexture = textureEl.attribute("src").as_string();

            if (textureEl.attribute("cropRect").as_string() != "")
            {
                tmpCrop = stringToSDLRect(textureEl.attribute("cropRect").as_string());
                cropRect = &tmpCrop;
            }
            else
            {
                cropRect = nullptr;
            }

            if (textureEl.attribute("outRect").as_string() != "")
            {
                tmpOut = stringToSDLRect(textureEl.attribute("outRect").as_string());
                outDim = &tmpOut;
            }
            else
            {
                outDim = nullptr;
            }
            
            AI *npc = new AI();
            SDL_Texture *npcTexture = TextureManager::loadTexture(pathToNPCTexture, cropRect, outDim);
            npc->init(npcEl.attribute("x").as_float(), npcEl.attribute("y").as_float(), npcTexture);
            npc->setAttribute("speed", npcEl.attribute("speed").as_float());
            Level::entities.push_back(npc);
        }
    }
}

void Level::renderBackground()
{
    if (Level::tiles.empty() || !isRectangularVector(Level::tiles))
    {
        std::cout << "Level::tiles is empty or invalid. Check if level was loaded properly." << std::endl;
        return;
    }

    // following code assumes that tiles are squares
    for (int y = 0; y < Level::tiles.size(); y++)
    {
        for (int x = 0; x < Level::tiles[0].size(); x++)
        {
            std::string tileName = Level::tiles[y][x];
            if (Level::tileDataLookup.find(tileName) != tileDataLookup.end())
            {
                SDL_Rect dst{x * Level::tileW, y * Level::tileH, tileW, tileH};
                Tile tileData = Level::tileDataLookup.at(tileName);
                SDL_RenderCopyEx(Game::renderer, tileData.texture, nullptr, &dst, tileData.isNatural ? 90 * ((x * y) % 3) : 0, nullptr, SDL_FLIP_NONE);
            }
            else
            {
                // error; tile does not exist
            }
        }
    }
}

void Level::generatePathfindingGrid()
{
    /*
    This function converts the data to a format that the pathfinding function uses.
    This should be called when switching to a new level, or anytime the available paths in the level changes.
    */
    Level::pathfindingGrid.clear();
    Level::pathfindingGrid = GridWithWeights{levelW, levelH};
    Tile currentTile;
    int rowIndex = 0, colIndex = 0;
    for (std::vector<std::string> tileRow : Level::tiles)
    {
        for (std::string tileName : tileRow)
        {
            currentTile = Level::getTileFromName(tileName);
            if (currentTile.movementCost < 0)
            {
                Level::pathfindingGrid.walls.insert(GridLocation{colIndex, rowIndex});
            }
            else
            {
                Level::pathfindingGrid.weights.insert_or_assign(GridLocation{colIndex, rowIndex}, currentTile.movementCost);
            }
            colIndex++;
        }
        rowIndex++;
    }
}

void Level::clean()
{
    // handles cleaning of entities as well, is this "doing more than one thing?"
    for (Entity *entity : Level::entities)
    {
        entity->~Entity();
    }
    Level::entities.clear();
}