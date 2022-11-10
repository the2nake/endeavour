#include "common.hpp"

#include "SDL.h"

#include <string>
#include <algorithm>
#include <cctype>
#include <locale>
#include <iostream>

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

std::string trimWhitespace(std::string s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                    { return !std::isspace(ch); }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                         { return !std::isspace(ch); })
                .base(),
            s.end());
    return s;
}

SDL_Rect stringToSDLRect(std::string s, std::string delim)
{
    SDL_Rect rect;

    if (s == "") {
        return rect; // empty rect
    }

    std::vector<std::string> temp;
    splitString(temp, s, delim);
    if (temp.size() >= 1)
    {
        rect.x = std::stoi(temp[0]);
    }
    if (temp.size() >= 2)
    {
        rect.y = std::stoi(temp[1]);
    }
    if (temp.size() >= 3)
    {
        rect.w = std::stoi(temp[2]);
    }
    if (temp.size() >= 4)
    {
        rect.h = std::stoi(temp[3]);
    }
    return rect;
}
