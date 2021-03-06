#include "AI.hpp"

#include "Game.hpp"
#include "Level.hpp"
#include "common.hpp"

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
    return f > 0.0f;
}

/**
 * Pathfinds to AI::target. If there is no target, clear the pathfinding variables
 */
void AI::pathfindToTarget()
{
    pathfindingTrace.clear();
    pathfindingCosts.clear();
    if (target == nullptr)
    {
        return;
    }
    float goalCenterX = target->getX() + target->getTextureW() / 2.0f;
    float goalCenterY = target->getY() + target->getTextureH() / 2.0f;
    GridLocation targetLocation = Level::getTilePosAt(goalCenterX, goalCenterY);
    GridLocation currentLocation = Level::getTilePosAt(x + texw / 2.0f, y + texh / 2.0f);
    auto currentLocationInPathIt = std::find(pathToGoal.begin(), pathToGoal.end(), currentLocation);
    if (pathTarget == initLocation || targetLocation != pathTarget || currentLocationInPathIt == pathToGoal.end())
    {
        // INCOMPLETE: Need to travel along the path to the AI if the AI is already at a certain point
        // --> if current position is in the list, check the item after
        // --> if not, go to pathToGoal[0]
        a_star_search(Level::pathfindingGrid, currentLocation, targetLocation, pathfindingTrace, pathfindingCosts);
        pathToGoal = getPathToLocation(pathfindingTrace, targetLocation);
        pathTarget = targetLocation;
        if (pathToGoal.size() > 1)
        {
            nextLocation = pathToGoal[1];
        }
        else
        {
            nextLocation = currentLocation;
        }
    }
    else
    {
        if (targetLocation == pathTarget && currentLocationInPathIt != pathToGoal.end())
        {
            if (isLast(currentLocationInPathIt, pathToGoal)) {
                return;
            } else {
                nextLocation = *std::next(currentLocationInPathIt);
            }
        }
    }
}

/**
 * @param location the grid cell to move to
 */
void AI::moveTowardsLocation(GridLocation location)
{
    // calculate some values needed for movement
    float nextCoordX = location.x * Level::tileW + 1;
    float nextCoordY = location.y * Level::tileH + 1;
    float dX = nextCoordX - x;
    float dY = nextCoordY - y;
    float cost = Level::getMovementCostInArea(x, y, texw, texh);
    float calculatedSpeed = getFloatAttribute("speed") / std::max(cost, 1.0f);
    float rootTwoOverTwo = 0.707106f;
    float diagStep = calculatedSpeed * rootTwoOverTwo;
    if (std::abs(dX) <= diagStep && std::abs(dY) <= diagStep)
    {
        x = nextCoordX;
        y = nextCoordY;
        return;
    }

    int xDirMult = isPositive(dX) ? 1 : -1;
    int yDirMult = isPositive(dY) ? 1 : -1;

    // snap to grid lines

    if (std::abs(dX) <= diagStep)
    {
        x = nextCoordX;
        y += yDirMult * calculatedSpeed - std::abs(dX);
    }
    else if (std::abs(dY) <= diagStep)
    {
        y = nextCoordY;
        x += xDirMult * calculatedSpeed - std::abs(dY);
    }
    else
    {
        x += xDirMult * diagStep;
        y += yDirMult * diagStep;
    }
}

void AI::correctPositioning()
{
    if (x < 0)
    {
        x = 0;
    }
    if (x + texw - 1 > Level::levelW * Level::tileW)
    {
        x = Level::levelW * Level::tileW - texw + 1;
    }
    if (y < 0)
    {
        y = 0;
    }
    if (y + texh - 1 > Level::levelH * Level::tileH)
    {
        y = Level::levelH * Level::tileH - texh + 1;
    }
}

void AI::update()
{
    // drawing coords
    drawX = std::floor(x);
    drawY = std::floor(y);

    // moving towards the target
    if (target != nullptr)
    {
        correctPositioning();

        bool nextLocationIsCurrentLocation = nextLocation == Level::getTilePosAt(x + texw / 2.0f, x + texh / 2.0f);
        bool nextLocationOutOfBounds = !Level::pathfindingGrid.inBounds(nextLocation);
        bool nextLocationHasNotBeenInit = nextLocation == initLocation;

        if (nextLocationIsCurrentLocation || nextLocationOutOfBounds || nextLocationHasNotBeenInit)
        {
            // generate the new location
            pathfindToTarget();
        }

        float nextCoordX = nextLocation.x * Level::tileW + 1;
        float nextCoordY = nextLocation.y * Level::tileH + 1;
        float dX = nextCoordX - x;
        float dY = nextCoordY - y;
        float cost = Level::getMovementCostInArea(x, y, texw, texh);
        float calculatedSpeed = getFloatAttribute("speed") / std::max(cost, 1.0f);
        float rootTwoOverTwo = 0.707106f;
        float diagStep = calculatedSpeed * rootTwoOverTwo;

        if (std::abs(dX) <= diagStep && std::abs(dY) <= diagStep)
        {
            x = nextCoordX;
            y = nextCoordY;
            pathfindToTarget();
            moveTowardsLocation(nextLocation);
        }
        else
        {
            moveTowardsLocation(nextLocation);
        }
    }
    // BUG: if the new path to the player is different, the enemies will stick to the old path, even when a more efficient path is available
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