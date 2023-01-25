
#include <iostream>
#include <string>
#include <algorithm>

#include "SDL.h"
#include "SDL_image.h"

#include "Game.hpp"
#include "TextureManager.hpp"
#include "Player.hpp"
#include "Level.hpp"

bool Game::running = true;
SDL_Window *Game::window = nullptr;
SDL_Renderer *Game::renderer = nullptr;
int Game::targetFrameTime = std::floor(1000.0 / 60.0);
int Game::frameTime = 0;

std::unordered_map<const char *, int> Game::errors;

std::unordered_map<int, bool> Game::keyIsDownMap;

std::queue<GridLocation> Game::tilesToHighlight = {};
SDL_Texture *Game::highlightTexture = nullptr;
int Game::numTilesToHighlight = 0;

Player *Game::player;

void initSDL()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        Game::running = false;
        std::cout << "SDL could not be fully initialized." << std::endl;
    }
}

void initSDLImage(int flags)
{
    if (IMG_Init(flags) != flags)
    {
        Game::running = false;
        std::cout << "SDL_image could not be fully initialized." << std::endl;
    }
}

void createWindow(std::string windowTitle, int w, int h, bool fullscreen, bool shown)
{
    Game::window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0) | (shown ? SDL_WINDOW_SHOWN : 0));
    if (!shown)
    {
        SDL_HideWindow(Game::window);
    }

    if (Game::window == nullptr)
    {
        Game::running = false;
        std::cout << "A window could not be created." << std::endl;
    }
}

void createRendererForWindow(SDL_Window *window)
{
    if (SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl") != SDL_TRUE)
    {
        std::cout << "The hint for the OpenGL renderer could not be set. Windows systems may experience rendering issues." << std::endl;
    }
    Game::renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (Game::renderer == nullptr)
    {
        Game::running = false;
        std::cout << "The renderer could not be created." << std::endl;
    }
    SDL_SetRenderDrawBlendMode(Game::renderer, SDL_BLENDMODE_BLEND);
}

Game::Game(std::string windowTitle, int w, int h, bool fullscreen, bool shown)
{
    SDL_ClearError();

    initSDL();
    initSDLImage(IMG_INIT_PNG | IMG_INIT_JPG);
    createWindow(windowTitle, w, h, fullscreen, shown);
    createRendererForWindow(Game::window);

    if (!Level::loadSave("DefaultPlayer", "save_1"))
        Game::clean();

    SDL_Rect crop{0, 0, 16, 16};
    SDL_Rect size{0, 0, Level::tileW, Level::tileH};
    SDL_Texture *highlightTexture = TextureManager::loadTexture("res/proposed/MiniWorldSprites/User Interface/BoxSelector.png", &crop, &size);

}

void Game::handleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            Game::running = false;
            break;
        case SDL_KEYDOWN:
            Game::keyIsDownMap.insert_or_assign(event.key.keysym.sym, true);
            break;
        case SDL_KEYUP:
            Game::keyIsDownMap.insert_or_assign(event.key.keysym.sym, false);
            break;
        default:
            break;
        }

        player->handleEvent(event);
    }

    std::string error = SDL_GetError();
    if (!error.empty())
    {
        add_error(error);
        SDL_ClearError();
    }

    // errors
    for (auto errorCooldownPair : Game::errors)
    {
        Game::errors.insert_or_assign(errorCooldownPair.first, errorCooldownPair.second - 1);
        if (errorCooldownPair.second <= 0)
        {
            Game::errors.erase(errorCooldownPair.first);
        }
    }
}

void Game::highlightTile(GridLocation tile)
{
    tilesToHighlight.push(tile);
    Game::numTilesToHighlight++;
}

void Game::update()
{
    player->update();

    for (Entity *entity : Level::entities)
    {
        entity->update();
    }
}

void Game::render()
{
    SDL_RenderClear(Game::renderer);

    Level::renderBackground();

    player->render();

    for (Entity *entity : Level::entities)
    {
        entity->render();
    }

    Level::renderForeground();

    while (Game::numTilesToHighlight > 0)
    {
        GridLocation tile = tilesToHighlight.front();
        SDL_Rect dst{tile.x * Level::tileW, tile.y * Level::tileH, Level::tileW, Level::tileH};
        SDL_RenderCopy(Game::renderer, Game::highlightTexture, nullptr, &dst);
        Game::tilesToHighlight.pop();
        Game::numTilesToHighlight--;
    }

    SDL_RenderPresent(Game::renderer);
}

void Game::add_error(std::string msg)
{
    if (!msg.empty() && Game::errors.find(msg.c_str()) == Game::errors.end())
    {
        std::cout << "\033[1;31mError:\033[0m " << msg.c_str() << std::endl;
        Game::errors.insert_or_assign(msg.c_str(), 60);
    }
}

void Game::clean()
{
    Game::running = false;

    SDL_DestroyWindow(Game::window);
    SDL_DestroyRenderer(Game::renderer);

    player->clean();
    Level::clean();

    SDL_Quit();
}