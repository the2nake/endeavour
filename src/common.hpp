#pragma once

#include "SDL.h"

#include <string>
#include <vector>

void splitString(std::vector<std::string> &container, std::string toSplit, std::string delim = " ");

std::string trimWhitespace(std::string s);

SDL_Rect stringToSDLRect(std::string s, std::string delim = " ");


// template function declaration and implementation can't be split
template <typename T>
bool isRectangularVector(std::vector<std::vector<T>> &v) {
    for (int i = 0; i < v.size(); i++) {
        if (v[i].size() != v[0].size()) {
            return false;
        }
    }
    return true;
}