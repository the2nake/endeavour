#pragma once
#include "Player.hpp"

#include "SDL.h"

#include <string>
#include <unordered_map>
// Game should only be initialised once

class Game {
public:
    Game(std::string windowTitle, int w, int h, bool fullscreen = false, bool shown = true);
    
    void handleEvents();
    void update();
    void render();

    void clean();

    static void add_error(std::string msg);
    static std::unordered_map<std::string, int> errors;

    static std::unordered_map<int, bool> keyIsDownMap;

    static Player player;

    static SDL_Window* window;
    static SDL_Renderer* renderer;
    static bool running;
    static double targetFrameTime;

private:

};