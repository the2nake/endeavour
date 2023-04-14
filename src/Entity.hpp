#pragma once

#include "SDL2/SDL.h"
#include <string>
#include <unordered_map>
#include <vector>

template <typename DataType>
using AttributeMap = std::unordered_map<std::string, DataType>;  // use attributes when change names is necessary, and performance is not critical

class Entity
{
public:
    Entity() {}
    virtual ~Entity();
    void init(float x, float y, SDL_Texture *texture);

    virtual void handleEvent(SDL_Event event);

    virtual void update();
    virtual void updateMovementDirection();
    virtual void updateAnimationState();
    virtual void updateTextures();
    
    virtual void render();

    virtual void clean();

    virtual SDL_Texture *getTexture() { return texture; }
    virtual float getX() { return x; }
    virtual float getY() { return y; }
    virtual int getTextureW() { return texw; }
    virtual int getTextureH() { return texh; }

    virtual int getIntAttribute(std::string name)
    {
        if (int_attrs.find(name) != int_attrs.end())
        {
            return int_attrs.at(name);
        }
        else
        {
            return -1;
        }
    }

    virtual float getFloatAttribute(std::string name)
    {
        if (flt_attrs.find(name) != flt_attrs.end())
        {
            return flt_attrs.at(name);
        }
        else
        {
            return -1;
        }
    }

    virtual std::string getStringAttribute(std::string name)
    {
        if (str_attrs.find(name) != str_attrs.end())
        {
            return str_attrs.at(name);
        }
        else
        {
            return "";
        }
    }

    virtual void setAttribute(std::string name, int value) { int_attrs.insert_or_assign(name, value); }
    virtual void setAttribute(std::string name, float value) { flt_attrs.insert_or_assign(name, value); }
    virtual void setAttribute(std::string name, std::string value) { str_attrs.insert_or_assign(name, value); }

    virtual void setAnimation(std::string animation);
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

    std::string currentAnimation = "";
    int currentAnimationFrame = 0;
    int msecsUntilNextFrame = 0;

    int timeSinceLastMovement = 10000;
    std::string dir = "s";
};