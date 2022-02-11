
#include <iostream>
#include <string>

#include "SDL.h"
#include "SDL_image.h"

#include "Game.hpp"

bool Game::running = true;
SDL_Window *Game::window = nullptr;
SDL_Renderer *Game::renderer = nullptr;

Player Game::player;

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

void createWindow(std::string windowTitle, int w, int h, bool fullscreen)
{
    Game::window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL | fullscreen);
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
}

Game::Game(std::string windowTitle, int w, int h, bool fullscreen)
{
    initSDL();
    initSDLImage(IMG_INIT_PNG | IMG_INIT_JPG);
    createWindow(windowTitle, w, h, fullscreen);
    createRendererForWindow(Game::window);
    SDL_Rect cropRect;
    cropRect.x = 0;
    cropRect.y = 0;
    cropRect.w = 10;
    cropRect.h = 10;
    player.init(1, 1, "res/tex/playerTexture.png", cropRect);
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

        default:
            break;
        }

        player.handleEvent(event);
    }
}

void Game::update()
{
    // player.update();
}

void Game::render()
{
    SDL_RenderClear(Game::renderer);

    player.render();

    SDL_RenderPresent(Game::renderer);
}
void Game::clean()
{
    SDL_DestroyWindow(Game::window);
    SDL_DestroyRenderer(Game::renderer);
    SDL_Quit();
}