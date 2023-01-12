#include "TextureManager.hpp"

#include "Game.hpp"

#include "SDL.h"
#include "SDL_image.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <map>

std::map<std::string, SDL_Texture *> TextureManager::cachedTextures;
bool TextureManager::textureCacheUsed = false; // used for testing

SDL_Texture *TextureManager::loadTexture(std::string pathToTexture, SDL_Rect *cropRect, SDL_Rect *outDim)
{
    SDL_Texture *croppedTexture = nullptr;
    SDL_Surface *sourceSurface = nullptr;
    SDL_Texture *sourceTexture = nullptr;
    if (std::filesystem::exists(pathToTexture.c_str()))
    {
        if (cachedTextures.find(pathToTexture) != cachedTextures.end())
        {
            sourceTexture = cachedTextures[pathToTexture];
            textureCacheUsed = true;
        }
        else
        {
            sourceSurface = IMG_Load(pathToTexture.c_str());
            sourceTexture = SDL_CreateTextureFromSurface(Game::renderer, sourceSurface);
            cachedTextures.insert_or_assign(pathToTexture, sourceTexture);
        }

        if (outDim != nullptr)
        {
            croppedTexture = SDL_CreateTexture(Game::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, outDim->w, outDim->h);
        }
        else if (cropRect != nullptr)
        {
            croppedTexture = SDL_CreateTexture(Game::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, cropRect->w, cropRect->h);
        }
        else
        {
            int srcw, srch;
            SDL_QueryTexture(sourceTexture, NULL, NULL, &srcw, &srch);
            croppedTexture = SDL_CreateTexture(Game::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, srcw, srch);
        }

        //SDL_SetTextureBlendMode(sourceTexture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(croppedTexture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(Game::renderer, croppedTexture);
        SDL_SetRenderDrawColor(Game::renderer, 0x00, 0x00, 0x00, SDL_ALPHA_TRANSPARENT);
        SDL_RenderClear(Game::renderer);
        SDL_RenderCopy(Game::renderer, sourceTexture, cropRect, nullptr);

        SDL_FreeSurface(sourceSurface);
        SDL_SetRenderTarget(Game::renderer, nullptr);
    }
    else
    {
        std::cout << "Texture with path " << pathToTexture << " could not be loaded as the file does not exist." << std::endl;
    }

    return croppedTexture;
}

SDL_Texture *TextureManager::retriveCachedTexture(std::string pathToTexture)
{
    if (cachedTextures.find(pathToTexture) != cachedTextures.end())
    {
        return cachedTextures[pathToTexture];
    }
    else
    {
        return nullptr;
    }
}