
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

    Game game = Game("Endeavour (pre-alpha)", 1024, 512, false, true);
    while (game.running)
    {
        auto start = std::chrono::high_resolution_clock::now();
        game.handleEvents();
        // std::cout << "Handle event time: " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << std::endl;
        // start = std::chrono::high_resolution_clock::now();
        game.update();
        // std::cout << "Update event time: " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << std::endl;
        // start = std::chrono::high_resolution_clock::now();
        game.render();
        // std::cout << "Render event time: " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << std::endl;
        // start = std::chrono::high_resolution_clock::now();
        auto end = std::chrono::high_resolution_clock::now();

        game.frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        //std::cout << game.frameTime << std::endl;
        SDL_Delay(std::max<int>(0, game.targetFrameTime - game.frameTime));

        game.frameTime = std::max(game.frameTime, game.targetFrameTime);
    }
    game.clean();
    return 0;
}