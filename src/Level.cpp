#include "Level.hpp"

#include "Entity.hpp"
#include "Game.hpp"
#include "TextureManager.hpp"
#include "common.hpp"

#include "pugixml.hpp"
#include "SDL.h"

#include <iostream>
#include <filesystem>

int Level::gridW = 0;
int Level::gridH = 0;
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
            int playerX = playerNode.attribute("x").as_int();
            int playerY = playerNode.attribute("y").as_int();
            xml_node firstFrameNode = playerNode.child("animation").child("frame");

            SDL_Rect cropRect = stringToSDLRect(firstFrameNode.attribute("crop").as_string());
            SDL_Rect outRect = stringToSDLRect(firstFrameNode.attribute("outRect").as_string());
            SDL_Texture *playerTexture = TextureManager::loadTexture(firstFrameNode.attribute("texture").as_string(), &cropRect, &outRect);

            Game::player = Player();
            Game::player.init(playerX, playerY, loadedPlayerName, playerTexture);
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
    std::string pathToSave = "saves/" + playerName + "/" + saveName + "/" + levelName + ".xml";
    xml_parse_result result = saveFile.load_file(pathToSave.c_str());
    xml_node levelEl, tileIndexEl, backgroundEl;
    Tile tempTile;
    if (result)
    {
        Level::tiles.clear();

        tileIndexEl = saveFile.child("tiles");

        levelEl = saveFile.child("level");
        backgroundEl = levelEl.child("background");

        Level::gridW = levelEl.attribute("gridW").as_int();
        Level::gridH = levelEl.attribute("gridH").as_int();

        for (xml_node tileEl : tileIndexEl.children("tile"))
        {
            SDL_Rect src = stringToSDLRect(tileEl.attribute("cropRect").as_string());
            SDL_Rect dst = stringToSDLRect(tileEl.attribute("outRect").as_string());
            tempTile.texture = TextureManager::loadTexture(tileEl.attribute("src").as_string(), &src, &dst); // this should point to the same texture pointed to in TextureManager::loadedTextures
                                                                                                             // if there are duplcicates
            tempTile.movementCost = tileEl.attribute("movementCost").as_int(1);
            tempTile.naturalTexture = tileEl.attribute("naturalTexture").as_bool();
            Level::tileDataLookup.insert_or_assign(tileEl.attribute("name").as_string(), tempTile);
        }

        int rowNum = 0;
        for (xml_node row : backgroundEl.children("row"))
        {
            tiles.push_back({});
            std::string rowText = row.text().as_string();
            std::vector<std::string> rowSplit = {};
            splitString(rowSplit, rowText, ",");
            for (std::string tileName : rowSplit)
            {
                Level::tiles[rowNum].push_back(trimWhitespace(tileName));
            }
            rowNum++;
        }

        // TODO: do the pathfinding weighting based on tile data
    }
    else
    {
        // problem loading save
        // check if file exists
        // check if file has proper syntax (single tags need to have </>, etc.)
    }
}

void Level::renderBackground()
{
    // TODO: make sure Level::tiles contains data and is a rectangle grid before continuing
    if (Level::tiles.empty() || !isRectangularVector(Level::tiles)) {
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
                SDL_Rect dst{x * Level::gridW, y * Level::gridH, gridW, gridH};
                Tile tileData = Level::tileDataLookup.at(tileName);
                SDL_RenderCopyEx(Game::renderer, tileData.texture, nullptr, &dst, tileData.naturalTexture ? 90 * ((x * y) % 3) : 0, nullptr, SDL_FLIP_NONE);
            }
            else
            {
                // error; tile does not exist
            }
        }
    }
}