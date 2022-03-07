#pragma once

#include "SDL.h"
#include <string>

class Entity {
public:
    Entity() {}
    void init(int x, int y, SDL_Texture* texture);

    virtual void handleEvent(SDL_Event event);
    virtual void update();
    virtual void render();

    virtual void clean();

    SDL_Texture* getTexture() {return texture;}
    int getX() {return x;}
    int getY() {return y;}

private:
    SDL_Texture *texture;
    int x = 0, y = 0;
    int texw = 0, texh = 0;
};