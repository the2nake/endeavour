#include "Player.hpp"

#include <iostream>
#include <string>
#include <cmath>
#include <chrono>

#include "SDL.h"

#include "TextureManager.hpp"
#include "Game.hpp"
#include "Level.hpp"
#include "common.hpp"
#include "Line.hpp"

std::string RESET_ALL_KEYBINDS = "";
std::vector<int> Player::registeredKeys;

Player::~Player()
{
    safelyDestroyTexture(this->texture);
    for (auto animation : animations)
    {
        for (auto tex : animation.second)
        {
            safelyDestroyTexture(tex);
        }
    }
}

void Player::init(float x, float y, SDL_Texture *texture)
{
    this->texture = texture;
    this->x = x;
    this->y = y;

    SDL_QueryTexture(texture, nullptr, nullptr, &texw, &texh);

    resetDefaultKeybind(RESET_ALL_KEYBINDS); // should replace with reading from text file and if file doesn't exist resetting the keybinds and saving them
    initAliasMap();
    initActionCooldowns();
}

void Player::initActionCooldowns()
{
    // NOTE: cooldowns under the targetFrameTime will not be noticeable
    defaultCooldowns.clear();
    defaultCooldowns.insert_or_assign("moveUp", 10.0);
    defaultCooldowns.insert_or_assign("moveDown", 10.0);
    defaultCooldowns.insert_or_assign("moveLeft", 10.0);
    defaultCooldowns.insert_or_assign("moveRight", 10.0);
}

void Player::initAliasMap()
{
    aliasFunctionMap.clear();
    aliasFunctionMap.insert_or_assign("callbackMoveUp", &Player::callbackMoveUp);
    aliasFunctionMap.insert_or_assign("callbackMoveDown", &Player::callbackMoveDown);
    aliasFunctionMap.insert_or_assign("callbackMoveRight", &Player::callbackMoveRight);
    aliasFunctionMap.insert_or_assign("callbackMoveLeft", &Player::callbackMoveLeft);
}

void Player::resetDefaultKeybind(std::string alias)
{
    if (alias != RESET_ALL_KEYBINDS)
    {
        int key = getDefaultKey(alias);
        if (key != -1)
        {
            keybinds.insert_or_assign(key, alias);
        }
    }
    else
    {
        keybinds.clear();
        // set the default keybinds for ALL of the keys
        keybinds.insert_or_assign(SDLK_w, "callbackMoveUp");
        keybinds.insert_or_assign(SDLK_s, "callbackMoveDown");
        keybinds.insert_or_assign(SDLK_a, "callbackMoveLeft");
        keybinds.insert_or_assign(SDLK_d, "callbackMoveRight");

        keybinds.insert_or_assign(SDLK_UP, "callbackMoveUp");
        keybinds.insert_or_assign(SDLK_DOWN, "callbackMoveDown");
        keybinds.insert_or_assign(SDLK_LEFT, "callbackMoveLeft");
        keybinds.insert_or_assign(SDLK_RIGHT, "callbackMoveRight");
    }
}

int Player::getDefaultKey(std::string alias)
{
    // returns the default key for an alias for a function pointer
    return -1;
}

playerAction Player::getFunctionOf(std::string alias)
{
    auto it = aliasFunctionMap.find(alias);
    if (it != aliasFunctionMap.end())
    {
        return it->second;
    }
    return nullptr;
}

void Player::callbackMoveRight() { dx += 1; }
void Player::callbackMoveLeft() { dx -= 1; }
void Player::callbackMoveDown() { dy += 1; }
void Player::callbackMoveUp() { dy -= 1; }

void Player::moveX(float mx)
{
    // TODO: add a check to see if there is a collision rectangle
    // if there is, check if the movement will actually conflict with any movement rectangles
    // - if it does conflict, move as close as possible to the left
    // - if it does not conflict, move normally
    // if there isn't, use the current logic (for just that tile)
    // do the same for the y axis
    if (mx == 0)
    {
        return;
    }

    bool movingLeft = mx < 0;
    int newx;
    if (movingLeft)
    {
        newx = std::floor(x + mx);
    }
    else
    {
        newx = std::ceil(x + mx);
    }

    int mcnw = Level::getTileFromName(Level::getTileNameAtPos("background", 0, newx, y)).movementCost;
    int mcne = Level::getTileFromName(Level::getTileNameAtPos("background", 0, newx + texw - 1, y)).movementCost;
    int mcse = Level::getTileFromName(Level::getTileNameAtPos("background", 0, newx + texw - 1, y + texh - 1)).movementCost;
    int mcsw = Level::getTileFromName(Level::getTileNameAtPos("background", 0, newx, y + texh - 1)).movementCost;

    bool nothingBlocking = mcnw > 0 && mcne > 0 && mcse > 0 && mcsw > 0;
    if (nothingBlocking)
    {
        x += mx;
    }
    else
    {
        if (movingLeft)
        {
            x = Level::tileW * std::floor(newx / Level::tileW) + Level::tileW;
        }
        else
        {
            x = Level::tileW * std::floor((newx + texw - 1) / Level::tileW) - texw;
        }
    }
}

void Player::moveY(float my)
{
    // TODO: add a check to see if there is a collision rectangle
    if (my == 0)
    {
        return;
    }

    bool movingUp = my < 0;
    int newy;
    if (movingUp)
    {
        newy = std::floor(y + my);
    }
    else
    {
        newy = std::ceil(y + my);
    }

    int mcnw = Level::getTileFromName(Level::getTileNameAtPos("background", 0, x, newy)).movementCost;
    int mcne = Level::getTileFromName(Level::getTileNameAtPos("background", 0, x + texw - 1, newy)).movementCost;
    int mcse = Level::getTileFromName(Level::getTileNameAtPos("background", 0, x + texw - 1, newy + texh - 1)).movementCost;
    int mcsw = Level::getTileFromName(Level::getTileNameAtPos("background", 0, x, newy + texh - 1)).movementCost;

    bool nothingBlocking = mcnw > 0 && mcne > 0 && mcse > 0 && mcsw > 0;
    if (nothingBlocking)
    {
        y += my;
    }
    else
    {
        if (movingUp)
        {
            y = Level::tileH * std::floor(newy / Level::tileH) + Level::tileH;
        }
        else
        {
            y = Level::tileH * std::floor((newy + texh - 1) / Level::tileH) - texh;
        }
    }
}

// this returns the grid coordinates of the tiles, 0-indexed
std::vector<GridLocation> getGridsOverlappedByLine(float x1, float y1, float x2, float y2)
{

    std::vector<GridLocation> tilesCovered;
    tilesCovered.push_back(Level::getPosAt(x1, y1));

    float dx = x2 - x1;
    float dy = y2 - y1;

    if (dx == 0 && dy == 0)
        return tilesCovered;

    float x = x1;
    float y = y1;

    bool steep = std::abs(dy) > std::abs(dx);

    float epsilon = 0.001f;

    while (std::abs(x2 - x) > epsilon || std::abs(y2 - y) > epsilon)
    {
        // check if line is steep to make sure the steps cover everything
        if (steep)
        {
            // if steep, step y by 1*Level::tileH and x by dx*Level::tileH/dy (which should be smaller than Level::tileH)
            // if y = 0, slope is not steep, so no need to check
            if (std::abs(y2 - y) < Level::tileH)
            {
                // if the distance between the target and the searched position is smaller than 1 tile
                x = x2;
                y = y2;
            }
            else
            {
                y += Level::tileH * dy / std::abs(dy); // dy / std::abs(dy) is the sign of dy (if it's decreasing, it's -1)
                x += dx * Level::tileH / std::abs(dy); // std::abs(dy) is just the scale factor, sign of dx determines direction. |dy| > |dx|.
            }
        }
        else
        {
            // if not steep, step x by 1*Level::tileW and y by dy*Level::tileW/dx (which should be smaller than Level::tileW)
            // if x = 0, then we use steep branch, so no need to check
            if (std::abs(x2 - x) < Level::tileW)
            {
                x = x2;
                y = y2;
            }
            else
            {
                x += Level::tileW * dx / std::abs(dx);
                y += dy * Level::tileW / std::abs(dx);
            }
        }

        tilesCovered.push_back(Level::getPosAt(x, y));
    }

    return tilesCovered;

    // INFO: loop: check if there should be an change in the slower changing direction (x if steep, y if not)
    //       if there is, check if the line also passes through the non-changing tile
    //       if there isn't, check if the square next to the current one should be returned

    //  ---------
    //  | t | x | c is current location
    //  |---|---| t is the changed tile coordinate
    //  | x | c | x are tiles need to be checked as well
    //  ---------
}

/*
    checks if the player will collided with something, given its future position
*/

bool Player::willBeColliding(float newx, float newy)
{
    SDL_Rect newPlayerRect{(int)(std::floor(newx)), (int)(std::floor(newy)), texw, texh};
    GridLocation newPlayerLoc = Level::getPosAt(newx, newy);
    std::queue<SDL_Rect> collisionRectangles;

    // auto start = std::chrono::high_resolution_clock::now();
    for (int j = newPlayerLoc.x; j <= newPlayerLoc.x + (int)std::floor(texw / Level::tileW); j++)
    {
        for (int k = newPlayerLoc.y; k <= newPlayerLoc.y + (int)std::floor(texh / Level::tileH); k++)
        {
            GridLocation loc{j, k};
            for (int layerCounter = 0; layerCounter < Level::getLayerCount(false); layerCounter++)
            {
                Tile tileAtLoc = Level::getTileFromName(Level::getTileNameAtGridPos("background", layerCounter, loc));
                if (tileAtLoc.movementCost <= 0)
                {
                    SDL_Rect collisionRect1 = tileAtLoc.collisionRect1;
                    collisionRect1.x += Level::tileW * loc.x;
                    collisionRect1.y += Level::tileH * loc.y;
                    SDL_Rect collisionRect2 = tileAtLoc.collisionRect2;
                    collisionRect2.x += Level::tileW * loc.x;
                    collisionRect2.y += Level::tileH * loc.y;
                    collisionRectangles.push(collisionRect1);
                    collisionRectangles.push(collisionRect2);
                }
            }
        }
    }
    // std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << std::endl;
    while (!collisionRectangles.empty())
    {
        if (rectIntersect(&newPlayerRect, &collisionRectangles.front()))
            return true;
        collisionRectangles.pop();
    }
    return false;
}

/*
    checks if the player is colliding with something
*/

bool Player::isColliding()
{
    return willBeColliding(this->x, this->y);
}

/*
Moves the player in a straight line mx units to the right and my units down.

This function implements collision detection.
*/

void Player::move(float mx, float my)
{
    // TODO: [urgent] unify moveX and moveY in a new algorithm

    // INFO: new algorithm
    // draw rays from the corners to their new positions <done>
    // for all tiles that the rays cover (using bresenham supercover algorithm), <done>
    //   check if the any of the rays intersect with the tile's boundaries (staring with the tiles closest to the player)
    // if there are no intersections
    //   move normally
    // else
    //   get the first intersection, then move there
    //   traverse along the boundary in the direction of motion until
    //      perpendicular displacement vector is completed
    //   or
    //      boundary is hit (check with 2 rays drawn from relevant corners)
    if (mx == 0 && my == 0)
        return;

    int stepNum = std::ceil(std::max(std::abs(mx), std::abs(my)) / 8); // to prevent inefficiency, do large steps, then step backwards slowly once you hit something
    float xStep = mx / stepNum;
    float yStep = my / stepNum;

    float newx = this->x;
    float newy = this->y;
    int xDir = std::signbit(mx) ? -1 : 1;
    int yDir = std::signbit(my) ? -1 : 1;

    bool xStopped = false, yStopped = false;
    for (int i = 0; i < stepNum; i++)
    {
        // get all tiles covered by the character

        // if intersecting boundary
        //     move x -xStep units
        //     if intersecting boundary
        //         move x xStep units
        //         move y -yStep units
        //         if intersecting boundary
        //             move x -xStep units
        //             continue
        //     else continue
        // else continue
        //
        // assuming starting from a non-intersecting position

        // check the boundaries at (j, k) grid location
        // move *backwards* to accomodate for them
        // stop moving in the x/y direction

        if (!xStopped && xStep != 0)
        {
            newx += xStep;
            if (willBeColliding(newx, newy))
            {
                do
                    newx -= xDir;
                while (willBeColliding(newx, newy));
                xStopped = true;
            }
        }

        if (!yStopped && yStep != 0)
        {
            newy += yStep;
            if (willBeColliding(newx, newy))
            {
                do
                    newy -= yDir;
                while (willBeColliding(newx, newy));
                yStopped = true;
            }
        }

        if (xStopped && yStopped)
            break;
    }

    x = newx;
    y = newy;
}

void Player::handleEvent(SDL_Event event)
{
    switch (event.type)
    {
    default:
        break;
    }
}

void Player::refreshRegisteredKeys()
{
    registeredKeys.clear(); // recheck which keys are registered
    for (auto it = Game::player->keybinds.begin(); it != Game::player->keybinds.end(); it++)
    {
        registeredKeys.push_back(it->first);
    }
}

void Player::setAnimation(std::string animation)
{
    if (animations.find(animation) != animations.end())
    {
        currentAnimation = animation;
    }
    else if (animations.find("idle") != animations.end())
    {
        currentAnimation = "idle";
    }
    else
    {
        currentAnimation = "";
    }
}

/**
 * @brief Updates the player's direction
 */

void Player::updateMovementDirection()
{
    bool currDirPermissable = false;

    if (dir == "n")
    {
        currDirPermissable = dy < 0;
    }
    else if (dir == "s")
    {
        currDirPermissable = dy > 0;
    }
    else if (dir == "e")
    {
        currDirPermissable = dx > 0;
    }
    else if (dir == "w")
    {
        currDirPermissable = dx < 0;
    }

    if (!currDirPermissable)
    {
        if (dx > 0)
        {
            dir = "e";
        }
        else if (dx < 0)
        {
            dir = "w";
        }
        else if (dy < 0)
        {
            dir = "n";
        }
        else if (dy > 0)
        {
            dir = "s";
        }
        msecsUntilNextFrame = 0;
    }
}

/**
 * Updates the character's animation based on movement data, shooting status, etc.
 */

void Player::updateAnimationState()
{
    if (animations.empty())
    {
        currentAnimation = "";
        return;
    }

    std::string oldAnimation = currentAnimation;

    if (timeSinceLastMovement < Game::targetFrameTime * 2)
    {
        setAnimation("walk" + dir);
    }
    else if (timeSinceLastMovement < 1000)
    {
        setAnimation("still" + dir);
    }
    else
    {
        setAnimation("idle" + dir);
    }

    if (oldAnimation != currentAnimation)
    {
        currentAnimationFrame = 0;
        // don't change msecUntilNextFrame to let the current animation finish
    }
}

/**
 * Updates the player's texture each frame
 */

void Player::updateTextures()
{
    if (currentAnimation == "")
    {
        return;
    }

    msecsUntilNextFrame -= Game::frameTime; // previous frame time in msec

    while (msecsUntilNextFrame <= 0)
    {
        msecsUntilNextFrame += std::max(animationDelays.at(currentAnimation)[currentAnimationFrame], Game::targetFrameTime);
        currentAnimationFrame = (currentAnimationFrame + 1) % animations.at(currentAnimation).size();
        this->texture = animations.at(currentAnimation)[currentAnimationFrame];
    }
}

void Player::update()
{
    drawX = std::floor(x); // make sure the thing draws on a pixel
    drawY = std::floor(y);
    dx = 0; // reset movement velocities
    dy = 0;

    // TODO: [low priority] only update registered keybinds when the controls are changed
    refreshRegisteredKeys();

    // update cooldowns
    for (auto &it : currentCooldowns)
    {
        it.second -= Game::targetFrameTime;
    }

    // executing the right function and handling the cooldowns
    std::unordered_map<std::string, double>::iterator defaultCooldownIter;
    for (int i = 0; i < registeredKeys.size(); i++)
    {
        auto checkPressIter = Game::keyIsDownMap.find(registeredKeys[i]);
        // check if the key has ever been pressed to prevent segmentation fault
        if (checkPressIter == Game::keyIsDownMap.end())
            continue;

        bool keyIsDown = checkPressIter->second;
        if (keyIsDown)
        {
            auto boundActionIter = keybinds.find(registeredKeys[i]);
            if (boundActionIter == keybinds.end())
                continue;

            auto currentCooldownIter = currentCooldowns.find(boundActionIter->second);
            if (currentCooldownIter == currentCooldowns.end() || (currentCooldownIter != currentCooldowns.end() && currentCooldownIter->second <= 0.0))
            {
                // execute the callback
                if (getFunctionOf(boundActionIter->second) != nullptr)
                {
                    (this->*(getFunctionOf(boundActionIter->second)))();
                }
                // deal with cooldown
                auto defaultCooldownIter = defaultCooldowns.find(boundActionIter->second);
                if (defaultCooldownIter != defaultCooldowns.end())
                {
                    currentCooldowns.insert_or_assign(boundActionIter->second, defaultCooldownIter->second);
                }
                else
                {
                    currentCooldowns.insert_or_assign(boundActionIter->second, 1.0);
                }
            }
        }
    }

    // averaging out movement costs at the four corners
    float movementCost = Level::getMovementCostInArea(x, y, texw, texh);
    float calculatedSpeed = getFloatAttribute("speed") / std::max(movementCost, 1.0f);

    if (std::abs(dx) > 0.5 || std::abs(dy) > 0.5)
    {
        timeSinceLastMovement = 0;
        float diagScaleFactor = sqrt(dx * dx + dy * dy);
        dx /= diagScaleFactor; // scaling horizontally
        dy /= diagScaleFactor; // scaling vertically
        // update the player's position
        move(dx * calculatedSpeed, dy * calculatedSpeed);

        updateMovementDirection();
    }
    else
    {
        timeSinceLastMovement += Game::frameTime;
    }

    updateAnimationState();
    updateTextures();
}

void Player::render()
{
    if (texture != nullptr)
    {
        SDL_Rect dst{drawX, drawY, texw, texh};
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_RenderCopy(Game::renderer, texture, nullptr, &dst);
    }
}

void Player::clean()
{
    this->~Player();
}