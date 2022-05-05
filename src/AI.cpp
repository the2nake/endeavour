#include "AI.hpp"

#include "Game.hpp"

#include <iostream>

AI::~AI() {
    clean();
}

void AI::init(int x, int y, SDL_Texture* texture) {
    this->texture = texture;
    this->x = x;
    this->y = y;

    SDL_QueryTexture(texture, nullptr, nullptr, &this->texw, &this->texh);
}

void AI::handleEvent(SDL_Event event) {

}

void AI::update() {
    
}

void AI::render() {
    SDL_Rect dst{x, y, texw, texh};
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(Game::renderer, texture, nullptr, &dst);
}

void AI::clean() {
    SDL_DestroyTexture(texture);
}