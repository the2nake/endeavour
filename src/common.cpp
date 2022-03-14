#include "common.hpp"

#include "SDL.h"

#include <string>

void splitString(std::vector<std::string> &container, std::string s, std::string delim)
{
    container.clear();
    size_t pos = 0;
    std::string token;
    s.append(delim); // trick to make the function include the portion after the final delimiter
    while ((pos = s.find(delim)) != std::string::npos)
    {
        token = s.substr(0, pos);
        container.push_back(token);
        s.erase(0, pos + delim.length());
    }
}

SDL_Rect stringToSDLRect(std::string s, std::string delim) {
    SDL_Rect rect;
    std::vector<std::string> temp;
    splitString(temp, s, delim);
    if (temp.size() >= 1) {
        rect.x = std::atoi(temp[0].c_str());
    }
    if (temp.size() >= 2) {
        rect.y = std::atoi(temp[1].c_str());
    }
    if (temp.size() >= 3) {
        rect.w = std::atoi(temp[2].c_str());
    }
    if (temp.size() >= 4) {
        rect.h = std::atoi(temp[3].c_str());
    }
    return rect;
}