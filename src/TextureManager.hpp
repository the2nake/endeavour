#pragma once

#include "SDL.h"

#include <string>
#include <unordered_map>

class TextureManager
{
public:
    static SDL_Texture *loadTexture(std::string path, SDL_Rect *cropRect = nullptr, SDL_Rect *outDim = nullptr);
    static void retriveCachedTexture(SDL_Texture *&target, std::string path);
    static bool textureCacheUsed; // used for testing
private:
    static std::unordered_map<std::string, SDL_Texture *> cachedTextures;
};
