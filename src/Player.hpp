#pragma once

#include <string>
#include <unordered_map>
//#include <functional>
#include <vector>

#include "SDL.h"

#include "Entity.hpp"

class Player
{
public:
    Player() {}
    void init(float x, float y, std::string name, SDL_Texture *texture);

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

    SDL_Texture *getTexture() { return texture; }
    float getX() { return x; }
    float getY() { return y; }
    std::string getName() { return name; }

    int getIntAttribute(std::string name)
    {
        if (int_attrs.find(name) != int_attrs.end())
        {
            return int_attrs.at(name);
        } else {
            return -1;
        }
    }

    float getFloatAttribute(std::string name)
    {
        if (flt_attrs.find(name) != flt_attrs.end())
        {
            return flt_attrs.at(name);
        } else {
            return -1;
        }
    }

    std::string getStringAttribute(std::string name)
    {
        if (str_attrs.find(name) != str_attrs.end())
        {
            return str_attrs.at(name);
        } else {
            return "";
        }
    }

    void setAttribute(std::string name, int value) { int_attrs.insert_or_assign(name, value); }
    void setAttribute(std::string name, float value) { flt_attrs.insert_or_assign(name, value); }
    void setAttribute(std::string name, std::string value) { str_attrs.insert_or_assign(name, value); }

private:
    SDL_Texture *texture;
    float x = 0, y = 0;
    int drawX = 0, drawY = 0;
    int texw = 0, texh = 0;
    std::string name;

    AttributeMap<int> int_attrs;
    AttributeMap<float> flt_attrs;
    AttributeMap<std::string> str_attrs;
};

typedef void (Player::*playerAction)();
