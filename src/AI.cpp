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

    SDL_QueryTexture(texture, nullptr, nullptr, &this->texw, &this->texh);
}

void AI::handleEvent(SDL_Event event)
{
}

void AI::update()
{
    /*
    // calculate next position
    int xPosOnGrid = x / Level::tileW; // because operands are ints, perform integer division
    int yPosOnGrid = y / Level::tileH;
    pathfindingCosts.clear();
    pathfindingTrace.clear();
    pathToGoal.clear();
    GridLocation currentGridPos{xPosOnGrid, yPosOnGrid};
    GridLocation playerGridPos{Game::player.getX() / Level::tileW, Game::player.getY() / Level::tileH};
s
    a_star_search<GridLocation, GridWithWeights>(Level::pathfindingGrid, currentGridPos, playerGridPos, pathfindingTrace, pathfindingCosts);
    pathToGoal = getPathToLocation(pathfindingTrace, playerGridPos);

    GridLocation nextTile;
    for (GridLocation tile : pathToGoal) {
        if (tile != currentGridPos) {
            nextTile = tile;
            break;
        }
    }

    int targetX = nextTile.x * Level::tileW;
    int targetY = nextTile.y * Level::tileH;
    // move towards player
    */
    drawX = std::round(x);
    drawY = std::round(y);
    float diffX = Game::player.getX() - x; // targetX - x;
    float diffY = Game::player.getY() - y; // targetY - y;

    Tile currentTile = Level::getTileFromName(Level::getTileNameAt(x, y));
    float calculatedSpeed = getFloatAttribute("speed") / currentTile.movementCost;

    if (std::abs(diffX) > 1 || std::abs(diffY) > 1)
    {
        float scaleFactor = calculatedSpeed / std::pow(diffX * diffX + diffY * diffY, 0.5);
        x += diffX * scaleFactor;
        y += diffY * scaleFactor;
    }

    // TODO: optimize pathfinding
    // TODO: fix pathfinding path being a less efficient path
    // TODO: implement last seen system
    // TODO: implement collisions
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