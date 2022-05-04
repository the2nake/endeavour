#include "Entity.hpp"
#include "SDL.h"
#include "Game.hpp"

Entity::~Entity() {
    Entity::clean();
}

void Entity::init(int x, int y, SDL_Texture* texture) {
    this->texture = texture;
    this->x = x;
    this->y = y;

    SDL_QueryTexture(texture, nullptr, nullptr, &texw, &texh);
}

void Entity::handleEvent(SDL_Event event) {

}

void Entity::update() {

}

void Entity::render() {
    SDL_Rect dst{x, y, texw, texh};
    SDL_RenderCopy(Game::renderer, texture, nullptr, &dst);
}

void Entity::clean() {
    SDL_DestroyTexture(texture);
}