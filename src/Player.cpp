#include "Player.hpp"

#include <iostream>
#include <string>
#include "SDL.h"

#include "TextureManager.hpp"
#include "Game.hpp"

std::string RESET_ALL_KEYBINDS = "";
std::vector<int> Player::registeredKeys;

void Player::init(int x, int y, std::string name, SDL_Texture *texture)
{
    this->texture = texture;
    this->x = x;
    this->y = y;
    this->name = name;

    SDL_QueryTexture(texture, nullptr, nullptr, &texw, &texh);

    resetDefaultKeybind(RESET_ALL_KEYBINDS); // should replace with reading from text file and if file doesn't exist resetting the keybinds and saving them
    initAliasMap();
}

void Player::initAliasMap()
{
    aliasFunctionMap.clear();
    aliasFunctionMap.insert_or_assign("moveUp", &Player::moveUp);
    aliasFunctionMap.insert_or_assign("moveDown", &Player::moveDown);
    aliasFunctionMap.insert_or_assign("moveRight", &Player::moveRight);
    aliasFunctionMap.insert_or_assign("moveLeft", &Player::moveLeft);
}

void Player::resetDefaultKeybind(std::string alias)
{
    if (alias != RESET_ALL_KEYBINDS)
    {
        int key = getDefaultKey(alias);
        if (key != -1)
        {
            keybinds.insert_or_assign(key, alias);
        }
    }
    else
    {
        keybinds.clear();
        // set the default keybinds for ALL of the keys
        keybinds.insert_or_assign(SDLK_w, "moveUp");
        keybinds.insert_or_assign(SDLK_s, "moveDown");
        keybinds.insert_or_assign(SDLK_a, "moveLeft");
        keybinds.insert_or_assign(SDLK_d, "moveRight");

        keybinds.insert_or_assign(SDLK_UP, "moveUp");
        keybinds.insert_or_assign(SDLK_DOWN, "moveDown");
        keybinds.insert_or_assign(SDLK_LEFT, "moveLeft");
        keybinds.insert_or_assign(SDLK_RIGHT, "moveRight");
    }
}

int Player::getDefaultKey(std::string alias)
{
    // returns the default key for an alias for a function pointer
    return -1;
}

Player::playerAction Player::getFunctionOf(std::string alias)
{
    auto it = aliasFunctionMap.find(alias);
    if (it != aliasFunctionMap.end())
    {
        return it->second;
    }
    return nullptr;
}

void Player::moveRight() { x += 1; }
void Player::moveLeft() { x -= 1; }
void Player::moveDown() { y += 1; }
void Player::moveUp() { y -= 1; }

void Player::handleEvent(SDL_Event event)
{
    switch (event.type)
    {
    default:
        break;
    }
}

void Player::update()
{
    registeredKeys.clear();
    for (std::unordered_map<int, std::string>::iterator it = Game::player.keybinds.begin(); it != Game::player.keybinds.end(); it++)
    {
        registeredKeys.push_back(it->first);
    }

    // cooldowns under the targetFrameTime will not be noticeable
    std::unordered_map<std::string, double>::iterator defaultCooldownIter;
    defaultCooldowns.insert_or_assign("moveUp", 10.0);
    defaultCooldowns.insert_or_assign("moveDown", 10.0);
    defaultCooldowns.insert_or_assign("moveLeft", 10.0);
    defaultCooldowns.insert_or_assign("moveRight", 10.0);

    for (auto &it : currentCooldowns)
    {
        it.second -= Game::targetFrameTime;
    }

    for (int i = 0; i < registeredKeys.size(); i++)
    {
        auto checkPressIter = Game::keyIsDownMap.find(registeredKeys[i]);
        if (checkPressIter == Game::keyIsDownMap.end())
        {
            continue;
        }

        bool keyIsDown = checkPressIter->second;
        if (!keyIsDown)
        {
            continue;
        }

        auto boundActionIter = keybinds.find(registeredKeys[i]);
        if (boundActionIter == keybinds.end())
        {
            continue;
        }

        auto currentCooldownIter = currentCooldowns.find(boundActionIter->second);
        if (currentCooldownIter == currentCooldowns.end() || (currentCooldownIter != currentCooldowns.end() && currentCooldownIter->second <= 0.0))
        {
            // execute
            if (getFunctionOf(boundActionIter->second) != nullptr)
            {
                (this->*(getFunctionOf(boundActionIter->second)))();
            }
            // deal with cooldown
            auto defaultCooldownIter = defaultCooldowns.find(boundActionIter->second);
            if (defaultCooldownIter != defaultCooldowns.end())
            {
                currentCooldowns.insert_or_assign(boundActionIter->second, defaultCooldownIter->second);
            }
            else
            {
                currentCooldowns.insert_or_assign(boundActionIter->second, 1.0);
            }
        }
    }
}

void Player::render()
{
    if (texture != nullptr)
    {
        SDL_Rect dst{x, y, texw, texh};
        SDL_RenderCopy(Game::renderer, texture, nullptr, &dst);
    }
}

void Player::clean()
{
    SDL_DestroyTexture(texture);
}