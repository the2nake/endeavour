#pragma once
#include <string>
#include <SDL2/SDL.h>

#include "Player.hpp"
// Game should only be initialised once

class Game {
public:
    Game(std::string windowTitle, int w, int h, bool fullscreen);
    
    void handleEvents();
    void update();
    void render();

    void clean();

    static Player player;

    static SDL_Window* window;
    static SDL_Renderer* renderer;
    static bool running;
private:
};