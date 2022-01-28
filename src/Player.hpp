#pragma once
#include <string>
#include <unordered_map>
#include <SDL2/SDL.h>

typedef void (*playerAction)(void);
typedef std::unordered_map<int, playerAction> keybindMap;

class Player
{
public:
    Player() {}
    void init(int x, int y, std::string pathToTexture, SDL_Rect cropRect);

    void handleEvent(SDL_Event event);
    void update();
    void render();

    void clean();

    keybindMap keybinds;

    // TODO: https://stackoverflow.com/questions/2136998/using-a-stl-map-of-function-pointers, for keybinds

private:
    SDL_Texture *texture;
    int x = 0, y = 0;
};
