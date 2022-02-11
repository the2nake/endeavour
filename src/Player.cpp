#include "Player.hpp"

#include <string>
#include "SDL.h"

#include "TextureManager.hpp"
#include "Game.hpp"

std::string RESET_ALL_KEYBINDS = "";

void Player::init(int x, int y, std::string pathToTexture, SDL_Rect cropRect)
{
    this->texture = TextureManager::loadTexture(pathToTexture, &cropRect);
    this->x = x;
    this->y = y;

    resetDefaultKeybind(RESET_ALL_KEYBINDS);
}

playerAction getFunctionOf(std::string alias) {
    // returns the function pointer referred to by an alias, hardcoded or use shared library to import references?
    return nullptr;
}

int getDefaultKeybind(std::string alias) {
    // returns the default key for an alias for a function pointer
    return -1;
}

void Player::resetDefaultKeybind(std::string alias) {
    if (alias != RESET_ALL_KEYBINDS) {
        int key = getDefaultKeybind(alias);
        if (key != -1) {
            keybinds.insert_or_assign(key, getFunctionOf(alias));
        }
    } else {
        keybinds.clear();
        // set the default keybinds for ALL of the keys
        keybinds.insert_or_assign(SDLK_w, &Player::moveUp);
        keybinds.insert_or_assign(SDLK_s, &Player::moveDown);
        keybinds.insert_or_assign(SDLK_a, &Player::moveLeft);
        keybinds.insert_or_assign(SDLK_d, &Player::moveRight);
    }
}

void Player::moveRight() { x += 10; }
void Player::moveLeft() { x -= 10; }
void Player::moveDown() { y += 10; }
void Player::moveUp() { y -= 10; }

void Player::handleEvent(SDL_Event event)
{
    keybindMap::iterator iter;
    switch (event.type)
    {
    case SDL_KEYDOWN:
        iter = keybinds.find(event.key.keysym.sym);
        if (iter != keybinds.end())
        {
            (this->*(iter->second))();
        }
        break;
    default:
        break;
    }
}

void Player::render()
{
    if (texture != nullptr)
    {
        int texW = 0, texH = 0;
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        SDL_Rect dst;
        dst.x = x;
        dst.y = y;
        dst.w = texW;
        dst.h = texH;
        SDL_RenderCopy(Game::renderer, texture, nullptr, &dst);
    }
}

void Player::clean()
{
    SDL_DestroyTexture(texture);
}