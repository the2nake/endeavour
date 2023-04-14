#pragma once

#include "SDL.h"

#include <string>
#include <vector>
#include <iosfwd>

namespace std {
    std::string to_string(SDL_Rect r);
};

bool approxEquals(double a, double b);

void splitString(std::vector<std::string> &container, std::string toSplit, std::string delim = " ");

std::string trimWhitespace(std::string s);

SDL_Rect stringToSDLRect(std::string s, std::string delim = " ");

// template function declaration and implementation can't be split
template <typename T>
bool isRectangularVector(std::vector<std::vector<T>> &v)
{
    for (uint i = 0; i < v.size(); i++)
    {
        if (v[i].size() != v[0].size())
        {
            return false;
        }
    }
    return true;
}

template <typename Iter, typename Cont>
bool isLast(Iter iter, const Cont &cont)
{
    return (iter != cont.end()) && (cont.end() - iter == 1);
}

// custom definition for floating point modulo that always returns a positive number
float floatingPointModulo(float a, float modB);

int orientation(float ax, float ay, float bx, float by, float cx, float cy);
bool onSegment(float ax, float ay, float bx, float by, float cx, float cy);

bool rectIntersect(SDL_Rect *r1, SDL_Rect *r2);
bool rectContainedInRect(SDL_Rect *r1, SDL_Rect *r2);

bool operator==(SDL_Rect rect, SDL_Rect rect2);

void safelyDestroyTexture(SDL_Texture *texture);
