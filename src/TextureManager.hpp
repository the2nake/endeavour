#pragma once

#include <string>
#include "SDL.h"

class TextureManager {
public:
    static SDL_Texture* loadTexture(std::string pathToTexture, SDL_Rect* cropRect = nullptr, SDL_Rect* outDim = nullptr);
};