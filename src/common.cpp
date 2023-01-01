#include "common.hpp"

#include "SDL.h"

#include <string>
#include <algorithm>
#include <cctype>
#include <locale>
#include <iostream>

#include "Line.hpp"

bool approxEquals(double a, double b)
{
    return std::abs(a - b) < 0.000001;
}

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

    if (s == "")
    {
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

float floatingPointModulo(float a, float modB)
{
    float modulo = a;
    if (a < 0)
    {
        while (modulo < 0)
        {
            modulo += modB;
        }
    }
    else
    {
        while (modulo - modB >= 0)
        {
            modulo -= modB;
        }
    }

    return modulo;
}

/*
Returns the orientation of the points A(ax, ay), B(bx, by), C(cx, cy)

0 means they are colinear
1 means they are clockwise
2 means they are anticlockwise
*/

int orientation(float ax, float ay, float bx, float by, float cx, float cy)
{
    float z = (ay - by) * (bx - cx) - (ax - bx) * (by - cy);
    if (z == 0)
        return 0;

    return (z > 0) ? 1 : 2;
}

bool onSegment(float ax, float ay, float bx, float by, float cx, float cy)
{
    if (bx <= std::max(ax, cx) && bx >= std::min(ax, cx) && by <= std::max(ay, cy) && by >= std::min(ay, cy))
        return true;

    return false;
}

/*
    Checks if r1 is contained in r2.
    r1 and r2 are represented with x and y anchors at the top-left
*/
bool rectContainedInRect(SDL_Rect *r1, SDL_Rect *r2)
{
    return r2->x < r1->x && r2->y < r1->y && r2->x + r2->w > r1->x + r1->w && r2->y + r2->h > r1->y + r1->h;
}

bool rectIntersect(SDL_Rect *r1, SDL_Rect *r2)
{
    // check if lines intersect
    // check if r1 is contained in r2, vice versa

    // lined n e s w
    std::vector<Line> r1lines = {Line(r1->x, r1->y, r1->x + r1->w, r1->y),
                                 Line(r1->x + r1->w - 1, r1->y, r1->x + r1->w - 1, r1->y + r1->h - 1),
                                 Line(r1->x, r1->y + r1->h - 1, r1->x + r1->w - 1, r1->y + r1->h - 1),
                                 Line(r1->x, r1->y, r1->x, r1->y + r1->h - 1)};

    // lined n e s w
    std::vector<Line> r2lines = {Line(r2->x, r2->y, r2->x + r2->w - 1, r2->y),
                                 Line(r2->x + r2->w - 1, r2->y, r2->x + r2->w - 1, r2->y + r2->h - 1),
                                 Line(r2->x, r2->y + r2->h - 1, r2->x + r2->w - 1, r2->y + r2->h - 1),
                                 Line(r2->x, r2->y, r2->x, r2->y + r2->h - 1)};

    for (Line line : r1lines)
    {
        for (Line line2 : r2lines)
        {
            if (line.intersectsLine(&line2))
                return true;
        }
    }

    if (rectContainedInRect(r1, r2))
        return true;
    if (rectContainedInRect(r2, r1))
        return true;

    return false;
}

bool operator==(SDL_Rect rect, SDL_Rect rect2)
{
    return rect.x == rect2.x && rect.y == rect2.y && rect.w == rect2.w && rect.h == rect2.h;
}
