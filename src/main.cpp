
#include "Game.hpp"

#include "SDL.h"

#include <filesystem>
#include <iostream>
#include <chrono>

int main(int argc, char *argv[])
{
    auto path = std::filesystem::current_path();
    bool workingDirIsCorrect = std::filesystem::exists(path.string() + "/res") &&
                               std::filesystem::exists(path.string() + "/saves");
    if (!workingDirIsCorrect)
    {
        std::cout << "The game has started in the wrong working directory. "
                  << "Attempting to correct working directory." << std::endl;
        std::filesystem::current_path(path.string() + "/..");
    }

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