#include "TextureManager.hpp"

#include "Game.hpp"

#include "SDL.h"
#include "SDL_image.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <unordered_map>

std::unordered_map<std::string, SDL_Texture *> TextureManager::cachedTextures;
bool TextureManager::textureCacheUsed = false; // used for testing

SDL_Texture *TextureManager::loadTexture(std::string path, SDL_Rect *crop, SDL_Rect *outDim)
{
    SDL_Texture *croppedTexture = nullptr;
    SDL_Surface *sourceSurface = nullptr;
    SDL_Texture *sourceTexture = nullptr;

    // check if this configuration has been used befor. if so point to the existing texture

    if (std::filesystem::exists(path.c_str()))
    {
        retriveCachedTexture(sourceTexture, path);
        if (sourceTexture != nullptr)
        {
            textureCacheUsed = true;
        }
        else
        {
            sourceSurface = IMG_Load(path.c_str());
            sourceTexture = SDL_CreateTextureFromSurface(Game::renderer, sourceSurface);
            cachedTextures.insert_or_assign(path, sourceTexture);
        }

        int resultw, resulth;
        if (outDim != nullptr)
        {
            resultw = outDim->w;
            resulth = outDim->h;
        }
        else if (crop != nullptr)
        {
            resultw = crop->w;
            resulth = crop->h;
        }
        else
            SDL_QueryTexture(sourceTexture, NULL, NULL, &resultw, &resulth);
        croppedTexture = SDL_CreateTexture(Game::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, resultw, resulth);

        SDL_SetTextureBlendMode(sourceTexture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(croppedTexture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(Game::renderer, croppedTexture);
        SDL_SetRenderDrawColor(Game::renderer, 0x00, 0x00, 0x00, SDL_ALPHA_TRANSPARENT);
        SDL_RenderClear(Game::renderer);
        SDL_RenderCopy(Game::renderer, sourceTexture, crop, nullptr);

        SDL_FreeSurface(sourceSurface);
        SDL_SetRenderTarget(Game::renderer, nullptr);
        // cachedTextures.insert_or_assign(textureQuery{path, crop, outDim}, sourceTexture);
    }
    else
    {
        std::cout << "Texture with path " << path << " could not be loaded as the file does not exist." << std::endl;
    }

    return croppedTexture;
}

void TextureManager::retriveCachedTexture(SDL_Texture *&target, std::string path)
{
    if (cachedTextures.find(path) != cachedTextures.end())
    {
        target = cachedTextures.at(path);
    }
    else
    {
        target = nullptr;
    }
}