#pragma once
#include "Entity.hpp"
#include <unordered_map>
#include <vector>
#include "pathfinding.hpp"

class AI : public Entity
{
public:
    AI() {}
    ~AI() override;
    void init(float x, float y, SDL_Texture *texture);

    void handleEvent(SDL_Event event) override;
    void update() override;
    void render() override;
    void clean() override;

    SDL_Texture *getTexture() override { return texture; }
    float getX() override { return x; }
    float getY() override { return y; }
    int getTextureW() override { return texw; }
    int getTextureH() override { return texh; }

    int getIntAttribute(std::string name) override
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

    float getFloatAttribute(std::string name) override
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

    std::string getStringAttribute(std::string name) override
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

    void setAttribute(std::string name, int value) override { int_attrs.insert_or_assign(name, value); }
    void setAttribute(std::string name, float value) override { flt_attrs.insert_or_assign(name, value); }
    void setAttribute(std::string name, std::string value) override { str_attrs.insert_or_assign(name, value); }

private:
    SDL_Texture *texture;
    float x = 0, y = 0;
    int drawX = 0, drawY = 0;
    int texw = 0, texh = 0;

    Entity* target = nullptr;

    AttributeMap<int> int_attrs;
    AttributeMap<float> flt_attrs;
    AttributeMap<std::string> str_attrs;

    void pathfindToTarget();
    void moveTowardsLocation(GridLocation loc);
    void correctPositioning();
    std::unordered_map<GridLocation, GridLocation> pathfindingTrace;
    std::unordered_map<GridLocation, double> pathfindingCosts;
    std::vector<GridLocation> pathToGoal;
    GridLocation pathTarget = initLocation;
    GridLocation nextLocation = initLocation;
    int pathIndex = -1;
};