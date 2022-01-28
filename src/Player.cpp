#include "Player.hpp"

#include <string>
#include <SDL2/SDL.h>

#include "TextureManager.hpp"
#include "Game.hpp"

void Player::init(int x, int y, std::string pathToTexture, SDL_Rect cropRect)
{
    this->texture = TextureManager::loadTexture(pathToTexture, &cropRect);
    this->x = x;
    this->y = y;
}

void Player::handleEvent(SDL_Event event)
{
    keybindMap::iterator iter;
    switch (event.type)
    {
    case SDL_KEYDOWN:
        iter = keybinds.find(event.key.keysym.sym);
        if (iter != keybinds.end()) {
            (*iter->second)(); // call whatever function that key is bound to.
        } else {
            // do nothing; the key isn't bound
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