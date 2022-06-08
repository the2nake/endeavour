#include "Entity.hpp"

#include "Game.hpp"

#include "SDL.h"

#include <iostream>

Entity::~Entity()
{
    clean();
}

void Entity::init(float x, float y, SDL_Texture *texture)
{
    this->texture = texture;
    this->x = x;
    this->y = y;

    SDL_QueryTexture(texture, nullptr, nullptr, &texw, &texh);
}

void Entity::handleEvent(SDL_Event event)
{
}

void Entity::update()
{
    drawX = std::floor(x);
    drawY = std::floor(y);
}

void Entity::render()
{
    SDL_Rect dst{drawX, drawY, texw, texh};
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(Game::renderer, texture, nullptr, &dst);
}

void Entity::clean()
{
    SDL_DestroyTexture(texture);
}