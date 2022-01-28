#pragma once

#include <string>
#include <SDL2/SDL.h>

class TextureManager {
public:
    static SDL_Texture* loadTexture(std::string pathToTexture, SDL_Rect* cropRect);
};