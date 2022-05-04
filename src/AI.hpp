#pragma once
#include "Entity.hpp"

class AI : public Entity {
public:
    AI() {}
    ~AI();
    void init(int x, int y, SDL_Texture* texture);

    void handleEvent(SDL_Event event) override;
    void update() override;
    //void render() override;
    //void clean() override;

    SDL_Texture* getTexture() {return texture;}
    int getX() {return x;}
    int getY() {return y;}

private:
    SDL_Texture *texture;
    int x = 0, y = 0;
    int texw = 0, texh = 0;
};