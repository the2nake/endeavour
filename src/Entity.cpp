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

void Entity::updateTextures()
{
    if (currentAnimation == "") {
        return;
    }

    msecsUntilNextFrame -= Game::frameTime; // previous frame time in msec

    if (msecsUntilNextFrame <= 0)
    {
        // TODO: update the this->texture when switching animations
        msecsUntilNextFrame = animationDelays.at(currentAnimation)[currentAnimationFrame];
        currentAnimationFrame += (currentAnimationFrame + 1) % animations.at(currentAnimation).size();
        // this->texture = animations.at(currentAnimation)[currentAnimationFrame];
    }

    // TODO: integrate animation with loading functions
    // TODO: document animations framework
    // NOTE: maybe I should update texw and texh? since it's related to collision
    //       or, I could just list that as unintended behaviour or something
    //       make sure to consider the performance impact as well
}

void Entity::updateAnimationState()
{
    if (animations.empty()) {
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

void Entity::update()
{
    drawX = std::floor(x);
    drawY = std::floor(y);

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