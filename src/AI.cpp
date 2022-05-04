#include "AI.hpp"

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

//void AI::render() {
//
//}
//
//void AI::clean() {
//    
//}