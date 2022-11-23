#include "Player.hpp"

#include <iostream>
#include <string>
#include "SDL.h"

#include "TextureManager.hpp"
#include "Game.hpp"
#include "Level.hpp"

std::string RESET_ALL_KEYBINDS = "";
std::vector<int> Player::registeredKeys;

Player::~Player()
{
    clean();
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

Player::playerAction Player::getFunctionOf(std::string alias)
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

    int mcnw = Level::getTileFromName(Level::getTileNameAtPosition("background", 0, newx, y)).movementCost;
    int mcne = Level::getTileFromName(Level::getTileNameAtPosition("background", 0, newx + texw - 1, y)).movementCost;
    int mcse = Level::getTileFromName(Level::getTileNameAtPosition("background", 0, newx + texw - 1, y + texh - 1)).movementCost;
    int mcsw = Level::getTileFromName(Level::getTileNameAtPosition("background", 0, newx, y + texh - 1)).movementCost;

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

    int mcnw = Level::getTileFromName(Level::getTileNameAtPosition("background", 0, x, newy)).movementCost;
    int mcne = Level::getTileFromName(Level::getTileNameAtPosition("background", 0, x + texw - 1, newy)).movementCost;
    int mcse = Level::getTileFromName(Level::getTileNameAtPosition("background", 0, x + texw - 1, newy + texh - 1)).movementCost;
    int mcsw = Level::getTileFromName(Level::getTileNameAtPosition("background", 0, x, newy + texh - 1)).movementCost;

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

void Player::handleEvent(SDL_Event event)
{
    switch (event.type)
    {
    default:
        break;
    }
}

void Player::update()
{
    drawX = std::floor(x); // make sure the thing draws on a pixel
    drawY = std::floor(y);
    dx = 0; // reset movement velocities
    dy = 0;
    // TODO: [low priority] only update registered keybinds when the controls are changed
    registeredKeys.clear(); // recheck which keys are registered
    for (auto it = Game::player.keybinds.begin(); it != Game::player.keybinds.end(); it++)
    {
        registeredKeys.push_back(it->first);
    }

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

    if (dx != 0 || dy != 0)
    {
        float diagScaleFactor = sqrt(dx * dx + dy * dy);
        dx /= diagScaleFactor; // scaling horizontally
        dy /= diagScaleFactor; // scaling vertically
        // update the player's position
        // TODO: [urgent] unify moveX and moveY in a new algorithm
        // new algorithm should calculated maximal horizontal and vertical bounds passed through raytracing
        // - trace rays from old corner to corner
        // - check which boundaries are crossed
        // - get the first boundary hit and align along that bound
        moveX(dx * calculatedSpeed);
        moveY(dy * calculatedSpeed);
    }
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
    SDL_DestroyTexture(texture);
}