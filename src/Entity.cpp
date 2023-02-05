#include "Entity.hpp"

#include "Game.hpp"
#include "common.hpp"

#include "SDL.h"

#include <iostream>

Entity::~Entity()
{
    safelyDestroyTexture(texture);
}

void Entity::init(float x, float y, SDL_Texture *texture)
{
    this->texture = texture;
    this->x = x;
    this->y = y;

    SDL_QueryTexture(texture, nullptr, nullptr, &texw, &texh);
}

void Entity::handleEvent(SDL_Event event)
{
}

void Entity::setAnimation(std::string animation)
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
 * @brief Updates the entity's direction
 */

void Entity::updateMovementDirection()
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

void Entity::updateAnimationState()
{
    if (animations.empty())
    {
        currentAnimation = "";
        return;
    }

    if (false)
    {
    }
    else
    {
        if (animations.find("idle") != animations.end())
        {
            currentAnimation = "idle";
        }
    }
}

void Entity::updateTextures()
{
    if (currentAnimation == "")
    {
        return;
    }

    msecsUntilNextFrame -= Game::frameTime; // previous frame time in msec

    while (msecsUntilNextFrame <= 0)
    {
        msecsUntilNextFrame += animationDelays.at(currentAnimation)[currentAnimationFrame];
        currentAnimationFrame = (currentAnimationFrame + 1) % animations.at(currentAnimation).size();
        this->texture = animations.at(currentAnimation)[currentAnimationFrame];
    }

    // TODO: integrate animation with loading functions
    // TODO: document animations framework
    // NOTE: maybe I should update texw and texh? since it's related to collision
    //       or, I could just list that as unintended behaviour or something
    //       make sure to consider the performance impact as well
}

void Entity::update()
{
    drawX = std::floor(x);
    drawY = std::floor(y);
    dx = 0; // reset movement velocities
    dy = 0;
    float startX = x;
    float startY = y;

    dx = x - startX;
    dy = y - startY;

    if (std::abs(dx) > 0.5 || std::abs(dy) > 0.5)
    {
        timeSinceLastMovement = 0;
        updateMovementDirection();
    }
    else
    {
        timeSinceLastMovement += Game::frameTime;
    }

    updateAnimationState();
    updateTextures();
}

void Entity::render()
{
    SDL_Rect dst{drawX, drawY, texw, texh};
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(Game::renderer, texture, nullptr, &dst);
}

void Entity::clean()
{
    this->~Entity();
}