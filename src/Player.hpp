#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "SDL.h"

class Player
{
public:
    Player() {}
    void init(int x, int y, std::string pathToTexture, SDL_Rect cropRect);

    void handleEvent(SDL_Event event);
    void update();
    void render();

    void clean();

    using keybindMap = std::unordered_map<int, void (Player::*)()>;
    keybindMap keybinds;
    
    void resetDefaultKeybind(std::string alias = "");

    void moveRight();
    void moveLeft();
    void moveDown();
    void moveUp();
    // TODO: https://stackoverflow.com/questions/2136998/using-a-stl-map-of-function-pointers, for keybinds

    SDL_Texture* getTexture() {return texture;}
    int getX() {return x;}
    int getY() {return y;}

private:
    SDL_Texture *texture;
    int x = 0, y = 0;
};

typedef void (Player::*playerAction)();
