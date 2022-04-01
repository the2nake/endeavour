#include <chrono>
#include "SDL.h"
#include "Game.hpp"


int main(int argc, char *argv[])
{
    double execTime = 9;
    Game game = Game("Endeavour: game with a stupid title", 1024, 512, false, true);
    while (Game::running)
    {
        auto start = std::chrono::high_resolution_clock::now();
        game.handleEvents();
        game.update();
        game.render();
        auto end = std::chrono::high_resolution_clock::now();

        execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        SDL_Delay(std::max<int>(0, Game::targetFrameTime - execTime));
    }
    game.clean();
    return 0;
}