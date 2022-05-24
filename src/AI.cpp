#include "AI.hpp"

#include "Game.hpp"
#include "Level.hpp"

#include <iostream>

AI::~AI()
{
    clean();
}

void AI::init(float x, float y, SDL_Texture *texture)
{
    this->texture = texture;
    this->x = x;
    this->y = y;

    SDL_QueryTexture(texture, nullptr, nullptr, &this->texw, &this->texh);
}

void AI::handleEvent(SDL_Event event)
{
}

void AI::update()
{
    // drawing coords
    drawX = std::round(x);
    drawY = std::round(y);
}

void AI::render()
{
    SDL_Rect dst{drawX, drawY, texw, texh};
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(Game::renderer, texture, nullptr, &dst);
}

void AI::clean()
{
    SDL_DestroyTexture(texture);
}