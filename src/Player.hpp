#pragma once

#include "Entity.hpp"

#include "SDL.h"

#include <string>
#include <unordered_map>
// #include <functional>
#include <vector>

class Player : public Entity
{
public:
    Player() {}
    ~Player() override;
    void init(float x, float y, SDL_Texture *texture);

    void handleEvent(SDL_Event event) override;

    void update() override;
    void updateAnimationState() override;
    void updateTextures() override;

    void render() override;

    void clean() override;

    std::unordered_map<int, std::string> keybinds;
    static std::vector<int> registeredKeys;
    void resetDefaultKeybind(std::string alias = "");
    void refreshRegisteredKeys();
    int getDefaultKey(std::string alias);

    using playerAction = void (Player::*)();
    playerAction getFunctionOf(std::string alias);
    std::unordered_map<std::string, playerAction> aliasFunctionMap;
    void initAliasMap();

    void callbackMoveRight();
    void callbackMoveLeft();
    void callbackMoveDown();
    void callbackMoveUp();

    void initActionCooldowns();
    std::unordered_map<std::string, double> defaultCooldowns; // in milliseconds
    std::unordered_map<std::string, double> currentCooldowns; // in milliseconds

    void move(float mx, float my);
    void moveX(float mx);
    void moveY(float my);

    SDL_Texture *getTexture() override { return texture; }
    float getX() override { return x; }
    float getY() override { return y; }
    int getTextureW() override { return texw; }
    int getTextureH() override { return texh; }

    int getIntAttribute(std::string name) override
    {
        if (int_attrs.find(name) != int_attrs.end())
            return int_attrs.at(name);
        else
            return -1;
    }

    float getFloatAttribute(std::string name) override
    {
        if (flt_attrs.find(name) != flt_attrs.end())
            return flt_attrs.at(name);
        else
            return -1;
    }

    std::string getStringAttribute(std::string name) override
    {
        if (str_attrs.find(name) != str_attrs.end())
            return str_attrs.at(name);
        else
            return "";
    }

    void setAttribute(std::string name, int value) override { int_attrs.insert_or_assign(name, value); }
    void setAttribute(std::string name, float value) override { flt_attrs.insert_or_assign(name, value); }
    void setAttribute(std::string name, std::string value) override { str_attrs.insert_or_assign(name, value); }

    void setAnimation(std::string animation);
    std::unordered_map<std::string, std::vector<SDL_Texture *>> animations;
    std::unordered_map<std::string, std::vector<int>> animationDelays;

private:
    SDL_Texture *texture = nullptr;
    float x = 0, y = 0, dx = 0, dy = 0;
    int drawX = 0, drawY = 0;
    int texw = 0, texh = 0;

    AttributeMap<int> int_attrs;
    AttributeMap<float> flt_attrs;
    AttributeMap<std::string> str_attrs;

    bool isColliding();
    bool willBeColliding(float x, float y);

    std::string currentAnimation = "";
    int currentAnimationFrame = 0;
    int msecsUntilNextFrame = 0;
    
    int timeSinceLastMovement = 10000;
    std::string dir = "s";
};

typedef void (Player::*playerAction)();
