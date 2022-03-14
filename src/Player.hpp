#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

#include "SDL.h"

class Player
{
public:
    Player() {}
    void init(int x, int y, std::string name, SDL_Texture* texture);

    void handleEvent(SDL_Event event);
    void update();
    void render();

    void clean();

    std::unordered_map<int, std::string> keybinds;
    static std::vector<int> registeredKeys;
    void resetDefaultKeybind(std::string alias = "");
    int getDefaultKey(std::string alias);
    using playerAction = void (Player::*)();
    playerAction getFunctionOf(std::string alias);

    std::unordered_map<std::string, void (Player::*)()> aliasFunctionMap;
    void initAliasMap();

    void moveRight();
    void moveLeft();
    void moveDown();
    void moveUp();

    std::unordered_map<std::string, double> defaultCooldowns; // in milliseconds
    std::unordered_map<std::string, double> currentCooldowns; // in milliseconds

    SDL_Texture* getTexture() {return texture;}
    int getX() {return x;}
    int getY() {return y;}
    std::string getName() {return name;}

private:
    SDL_Texture *texture;
    int x = 0, y = 0;
    int texw = 0, texh = 0;
    std::string name;
};

typedef void (Player::*playerAction)();
