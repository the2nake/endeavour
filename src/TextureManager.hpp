#pragma once

#include "SDL.h"

#include <string>
#include <map>

class TextureManager
{
public:
    static SDL_Texture *loadTexture(std::string pathToTexture, SDL_Rect *cropRect = nullptr, SDL_Rect *outDim = nullptr);
    static SDL_Texture *retriveCachedTexture(std::string pathToTexture);
    static bool textureCacheUsed; // used for testing
private:
    static std::map<std::string, SDL_Texture *> cachedTextures;
};