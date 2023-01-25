#include "Level.hpp"

#include "Entity.hpp"
#include "AI.hpp"
#include "Game.hpp"
#include "TextureManager.hpp"
#include "common.hpp"

#include "pugixml.hpp"
#include "SDL.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <algorithm>

int Level::tileW = 0;
int Level::tileH = 0;
int Level::levelW = 0;
int Level::levelH = 0;

GridWithWeights Level::pathfindingGrid{0, 0};

std::vector<Entity *> Level::entities;
std::vector<std::vector<std::vector<std::string>>> Level::background;
std::vector<std::vector<std::vector<std::string>>> Level::foreground;

std::unordered_map<std::string, Tile> Level::tileDataLookup;
std::vector<std::string> Level::transparentTiles;

template <typename entityType>
bool Level::loadAnimations(Entity *entity, pugi::xml_node entityNode)
{
    entityType typedEntity = dynamic_cast<entityType>(entity);

    for (auto animationNode : entityNode.children("animation"))
    {

        std::string name = animationNode.attribute("name").as_string();
        if (name != "")
        {
            // std::cout << "Loading animation named " << name << " of entity " << typedEntity->getStringAttribute("name") << std::endl;
            typedEntity->animations.insert_or_assign(name, std::vector<SDL_Texture *>{});
            typedEntity->animationDelays.insert_or_assign(name, std::vector<int>{});

            for (auto frameNode : animationNode.children("frame"))
            {
                SDL_Rect cropRect = stringToSDLRect(frameNode.attribute("cropRect").as_string());
                SDL_Rect outRect = stringToSDLRect(frameNode.attribute("outRect").as_string());
                // std::cout << frameNode.attribute("src").as_string() << std::endl;
                // std::cout << std::to_string(cropRect) << std::endl;
                // std::cout << std::to_string(outRect) << std::endl;
                typedEntity->animations.at(name).push_back(TextureManager::loadTexture(frameNode.attribute("src").as_string(), &cropRect, &outRect));
                typedEntity->animationDelays.at(name).push_back(frameNode.attribute("delay").as_int(100));
            }
        }
    }

    return true;
}

/**
 * Loads a save at `saves/playerName/saveName/player.xml`
 * Returns false if there are issues, else returns true
 */

bool Level::loadSave(std::string playerName, std::string saveName)
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
            SDL_Texture *playerTexture = TextureManager::loadTexture(firstFrameNode.attribute("src").as_string(), &cropRect, &outRect);
            Game::player = new Player();
            Game::player->init(playerX, playerY, playerTexture);
            Game::player->setAttribute("name", playerNode.attribute("name").as_string());
            Game::player->setAttribute("speed", playerNode.attribute("speed").as_string() == "" ? (float)(1) : playerNode.attribute("speed").as_float());

            if (!Level::loadAnimations<Player *>(Game::player, playerNode)) {
                return false;
            }

            xml_node levelPointerNode = playerFile.child("level");
            std::string levelName = levelPointerNode.attribute("name").as_string();
            if (levelName != "")
            {
                return Level::loadLevel(playerName, saveName, levelName);
            }
            else
            {
                return false;
            }
        }
        else
        {
            // problem loading save
            Game::add_error("Cannot load save at " + pathToSave + ".");
            return false;
        }
    }
    else
    {
        Game::add_error("Save file at " + pathToSave + " could not be found.");
        return false;
    }
}

/**
 * Loads a level and generates its pathfinding grid
 * Usually called from loadSave
 * Returns false if there's a problem, otherwise returns true
 */

bool Level::loadLevel(std::string playerName, std::string saveName, std::string levelName)
{
    using namespace pugi;

    xml_document saveFile;
    std::string pathToSave = "saves/" + playerName + "/" + saveName + "/levels/" + levelName + ".xml";
    xml_parse_result result = saveFile.load_file(pathToSave.c_str());
    xml_node levelEl, tileIndexEl;
    Tile tempTile;
    if (result)
    {
        // if the xml file is valid, do some setup
        tileIndexEl = saveFile.child("tiles");
        levelEl = saveFile.child("level");

        // record the width and height of each tile
        Level::tileW = levelEl.attribute("tileW").as_int();
        Level::tileH = levelEl.attribute("tileH").as_int();

        // set the window size
        SDL_SetWindowSize(Game::window, levelEl.attribute("width").as_int() * Level::tileW, levelEl.attribute("height").as_int() * Level::tileH);

        // recentre the window
        SDL_SetWindowPosition(Game::window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

        // load the tile data
        for (xml_node tileEl : tileIndexEl.children("tile"))
        {
            // for each tile, deserialise the data

            // get the rectangles
            SDL_Rect src = stringToSDLRect(tileEl.attribute("cropRect").as_string());
            SDL_Rect dst = stringToSDLRect(tileEl.attribute("outRect").as_string());

            // get the texture pointer
            tempTile.texture = TextureManager::loadTexture(tileEl.attribute("src").as_string(), &src, &dst); // this should point to the same texture pointed to in TextureManager::loadedTextures
                                                                                                             // if there are duplcicate references

            if (tileEl.attribute("transparent").as_bool())
                Level::transparentTiles.push_back(tileEl.attribute("name").as_string());

            // get movement cost
            tempTile.movementCost = tileEl.attribute("movementCost").as_float(1.0f);

            // get the natural flag
            tempTile.isNatural = tileEl.attribute("isNatural").as_bool(false);

            // collision rectangles
            tempTile.collisionRect1 = stringToSDLRect(tileEl.attribute("collisionRect").as_string());
            tempTile.collisionRect2 = stringToSDLRect(tileEl.attribute("collisionRect2").as_string());

            if (tempTile.movementCost <= 0)
                if (tileEl.attribute("collisionRect").as_string() == "" && tileEl.attribute("collisionRect2").as_string() == "")
                    tempTile.collisionRect1 = {0, 0, Level::tileW, Level::tileH};

            // store the data into the lookup table
            Level::tileDataLookup.insert_or_assign(tileEl.attribute("name").as_string(), tempTile);
        }

        // make sure the tiles are empty
        Level::background.clear();
        Level::foreground.clear();

        // load the actual tile locations
        int rowNum;
        int backgroundLayerIndex = 0;
        for (xml_node backgroundEl : levelEl.children("background"))
        {
            Level::background.push_back({});
            rowNum = 0;
            for (xml_node row : backgroundEl.children("row"))
            {
                Level::background[backgroundLayerIndex].push_back({});
                std::string rowText = row.text().as_string();
                std::vector<std::string> rowSplit;
                splitString(rowSplit, rowText, ",");
                Level::levelW = 0;
                for (std::string tileName : rowSplit)
                {
                    Level::background[backgroundLayerIndex][rowNum].push_back(trimWhitespace(tileName));
                    Level::levelW += 1;
                }
                rowNum++;
            }
            backgroundLayerIndex++;
        }
        Level::levelH = rowNum;

        int foregroundLayerIndex = 0;
        for (xml_node foregroundEl : levelEl.children("foreground"))
        {
            Level::foreground.push_back({});
            rowNum = 0;
            for (xml_node row : foregroundEl.children("row"))
            {
                Level::foreground[foregroundLayerIndex].push_back({});
                std::string rowText = row.text().as_string();
                std::vector<std::string> rowSplit;
                splitString(rowSplit, rowText, ",");
                for (std::string tileName : rowSplit)
                {
                    Level::foreground[foregroundLayerIndex][rowNum].push_back(trimWhitespace(tileName));
                }
                rowNum++;
            }
            foregroundLayerIndex++;
        }

        Level::generatePathfindingGrid();

        return Level::loadNPCs(playerName, saveName, levelName);
    }
    else
    {
        // problem loading save
        // check if file exists
        // check if file has proper syntax (single tags need to have </>, etc.)

        Game::add_error("Save file referenced in saves/" + playerName + "/" + saveName + "/player.xml is invalid or does not exist.");
        return false;
    }
}

/**
 * Loads all the npcs in a level
 * Usually called from loadLevel
 * Returns false if loading fails, otherwise returns true
 */

bool Level::loadNPCs(std::string playerName, std::string saveName, std::string levelName)
{
    using namespace pugi;

    xml_document npcFile, levelFile;
    std::string npcFileFolder = "saves/" + playerName + "/" + saveName + "/npcs/";
    std::string levelFilePath = "saves/" + playerName + "/" + saveName + "/levels/" + levelName + ".xml";
    xml_parse_result result = levelFile.load_file(levelFilePath.c_str());

    if (result)
    {
        std::vector<std::string> npcsToLoad = {};
        for (auto npcListingEl : levelFile.child("npcs").children("npc"))
        {
            npcsToLoad.push_back(npcListingEl.attribute("id-name").as_string());
        }

        if (npcsToLoad.empty())
        {
            return true;
        }

        for (std::string npcName : npcsToLoad)
        {
            std::string pathToSave = npcFileFolder + npcName + ".xml";
            xml_parse_result npcFileResult = npcFile.load_file(pathToSave.c_str());

            if (npcFileResult)
            {
                // Valid npc files are: adjacent to this level, have proper format, and contain at least one npc
                xml_node npcNode = npcFile.child("npc");
                xml_node textureEl = npcNode.child("animation").child("frame");
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
                    npc->init(npcNode.attribute("x").as_float(), npcNode.attribute("y").as_float(), npcTexture);
                    npc->setAttribute("name", npcNode.attribute("name").as_string());
                    npc->setAttribute("speed", npcNode.attribute("speed").as_float());

                    if (!Level::loadAnimations<AI *>(npc, npcNode))
                    {
                        return false;
                    }

                    Level::entities.push_back(npc);
                }
            }
            else
            {
                Game::add_error("Npc file at " + pathToSave + " referenced by " + levelFilePath + " is invalid or does not exist.");
                return false;
            }
        }

        return true;
    }
    else
    {
        // Level file not found
        Game::add_error("Level file at " + levelFilePath + " is invalid or does not exist.");
        return false;
    }
}

void Level::renderLayer(std::string layer)
{
    auto layerDataRef = &background;
    if (layer == "foreground")
        layerDataRef = &foreground;

    if (layerDataRef->empty())
    {
        Game::add_error("Tile data is empty or invalid.");
        return;
    }

    // TODO: cache the frames for each layer

    for (auto layer : *layerDataRef)
    {
        if (!isRectangularVector(layer))
        {
            Game::add_error("Tile data is empty or invalid.");
            return;
        }

        // following code assumes that tiles are squares
        int tileX = 0;
        int tileY = 0;
        for (auto row : layer)
        {
            for (auto tileName : row)
            {
                if (std::find(Level::transparentTiles.begin(), Level::transparentTiles.end(), tileName) != transparentTiles.end())
                {
                    tileX++;
                    continue;
                }

                if (Level::tileDataLookup.find(tileName) != tileDataLookup.end())
                {
                    SDL_Rect dst{tileX * Level::tileW, tileY * Level::tileH, Level::tileW, Level::tileH};
                    Tile tileData = Level::tileDataLookup[tileName];
                    if (tileData.isNatural)
                        SDL_RenderCopyEx(Game::renderer, tileData.texture, nullptr, &dst, tileData.isNatural ? 90 * ((tileX * tileY) % 3) : 0, nullptr, SDL_FLIP_NONE);
                    else
                        SDL_RenderCopy(Game::renderer, tileData.texture, nullptr, &dst);
                }
                else
                {
                    Game::add_error("Tile with name " + (std::string)tileName + " is undefined.");
                }
                tileX++;
            }
            tileY++;
            tileX = 0;
        }
    }
}

void Level::renderBackground()
{
    if (!background.empty())
        renderLayer("background");
}

void Level::renderForeground()
{
    if (!foreground.empty())
        renderLayer("foreground");
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
    for (auto tileRow : Level::background[0])
    {
        colIndex = 0;
        for (auto tileName : tileRow)
        {
            GridLocation loc{colIndex, rowIndex};
            currentTile = Level::getTileFromName(tileName);
            if (currentTile.movementCost <= 0)
            {
                Level::pathfindingGrid.walls.insert(loc);
                Level::pathfindingGrid.setCost(loc, 0);
            }
            else if (pathfindingGrid.walls.find(loc) != pathfindingGrid.walls.end())
            {
                pathfindingGrid.walls.extract(loc);
                Level::pathfindingGrid.setCost(loc, currentTile.movementCost);
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
        entity->clean();
    }
    Level::entities.clear();
}