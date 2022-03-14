#include "Level.hpp"

#include "Entity.hpp"
#include "Game.hpp"
#include "TextureManager.hpp"
#include "common.hpp"

#include "pugixml.hpp"
#include "SDL.h"

#include <iostream>
#include <filesystem>

std::vector<Entity *> Level::entities;
std::unordered_map<std::string, Tile> Level::tileDataLookup;
std::vector<std::vector<std::string>> Level::tiles;

void Level::loadPlayerData(std::string playerName, std::string saveName)
{
    std::string pathToSave = "saves/" + playerName + "/" + saveName + "/player.xml";
    if (std::filesystem::exists(pathToSave.c_str()))
    {
        pugi::xml_document playerFile;
        pugi::xml_parse_result result = playerFile.load_file(pathToSave.c_str());
        pugi::xml_node playerNode = playerFile.child("player");

        std::string loadedPlayerName = playerNode.attribute("name").as_string();
        int playerX = playerNode.attribute("x").as_int();
        int playerY = playerNode.attribute("y").as_int();
        pugi::xml_node firstFrameNode = playerNode.child("animation").child("frame");

        SDL_Rect cropRect = stringToSDLRect(firstFrameNode.attribute("crop").as_string());
        SDL_Rect outRect = stringToSDLRect(firstFrameNode.attribute("outRect").as_string());
        SDL_Texture *playerTexture = TextureManager::loadTexture(firstFrameNode.attribute("texture").as_string(), &cropRect, &outRect);

        Game::player = Player();
        Game::player.init(playerX, playerY, loadedPlayerName, playerTexture);
    } else {
        std::cout << "Save file at " + pathToSave + " could not be found." << std::endl;
    }
}

void Level::loadLevel(std::string pathToLevelData)
{
    pugi::xml_document saveFile;
    pugi::xml_parse_result result = saveFile.load_file(pathToLevelData.c_str());

    std::cout << saveFile.child("npc").attribute("name").value() << std::endl;
}
