#include "AI.hpp"

#include "Game.hpp"
#include "Level.hpp"

#include <iostream>

AI::~AI()
{
    clean();
}

void AI::init(float x, float y, SDL_Texture *texture)
{
    this->texture = texture;
    this->x = x;
    this->y = y;

    SDL_QueryTexture(texture, nullptr, nullptr, &texw, &texh);

    // temp
    this->target = &Game::player;
}

void AI::handleEvent(SDL_Event event)
{
}

bool isPositive(float f)
{
    return !std::signbit(f);
}

void AI::update()
{
    // drawing coords
    drawX = std::round(x);
    drawY = std::round(y);

    // pathfind if the thing is stationary
    pathfindingTrace.clear();
    pathfindingCosts.clear();

    float goalCenterX = target->getX() + target->getTextureW() / 2.0f;
    float goalCenterY = target->getY() + target->getTextureH() / 2.0f;
    GridLocation targetLocation = Level::getTilePosAt(goalCenterX, goalCenterY);
    GridLocation currentLocation = Level::getTilePosAt(x + texw / 2.0f, y + texh / 2.0f);
    GridLocation nextLocation = currentLocation;
    a_star_search(Level::pathfindingGrid, currentLocation, targetLocation, pathfindingTrace, pathfindingCosts);

    auto path = getPathToLocation(pathfindingTrace, targetLocation);
    if (path.size() > 0)
    {
        for (GridLocation location : path)
        {
            if ((location.x != currentLocation.x || location.y != currentLocation.y) && Level::pathfindingGrid.in_bounds(location))
            {
                nextLocation = location;
                break;
            }
        }
        if (nextLocation != currentLocation) // && Level::pathfindingGrid.in_bounds(nextLocation))
        {
            float nextCoordX = nextLocation.x * Level::tileW + 1;
            float nextCoordY = nextLocation.y * Level::tileH + 1;
            float dX = nextCoordX - x;
            float dY = nextCoordY - y;
            float cost = Level::getMovementCostInArea(x, y, texw, texh);
            float calculatedSpeed = getFloatAttribute("speed") / std::max(cost, 1.0f);
            float rootTwoOverTwo = 0.707106f;
            float diagStep = calculatedSpeed * rootTwoOverTwo;

            int xDirMult = isPositive(dX) ? 1 : -1;
            int yDirMult = isPositive(dY) ? 1 : -1;

            if (std::abs(dX) <= diagStep / 2.0f && std::abs(dY) <= diagStep / 2.0f)
            {
                x = nextCoordX;
                y = nextCoordY;
            }
            else if (std::abs(dX) <= diagStep / 2.0f)
            {
                x = nextCoordX;
                y += yDirMult * calculatedSpeed;
            }
            else if (std::abs(dY) <= diagStep / 2.0f)
            {
                y = nextCoordY;
                x += xDirMult * calculatedSpeed;
            }
            else
            {
                x += xDirMult * diagStep;
                y += yDirMult * diagStep;
            }
        }
    }
}

void AI::render()
{
    SDL_Rect dst{drawX, drawY, texw, texh};
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(Game::renderer, texture, nullptr, &dst);
}

void AI::clean()
{
    SDL_DestroyTexture(texture);
}