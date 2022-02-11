#include <chrono>
#include "SDL.h"
#include "Game.hpp"

double frameTime = 1000.0 / 60.0;
double execTime = 0;

int main(int argc, char *argv[])
{
    Game game = Game("Endeavour: game with a stupid title", 800, 600, false);
    while (Game::running)
    {
        auto start = std::chrono::high_resolution_clock::now();
        game.handleEvents();
        game.update();
        game.render();
        auto end = std::chrono::high_resolution_clock::now();

        execTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        SDL_Delay(std::max<int>(0, std::round(frameTime - execTime)));
    }
    game.clean();
    return 0;
}