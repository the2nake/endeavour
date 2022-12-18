#include "Line.hpp"

#include <cmath>

#include "common.hpp"

Line::Line(int x1, int y1, int x2, int y2)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}

Line::Line(float x1, float y1, float x2, float y2)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}

float Line::getSlope()
{
    if (x2 - x1 == 0)
    {
        return 1000000.0;
    }

    return (y2 - y1) / (x2 - x1);
}

float Line::getAngleFromPosXAxis(bool radians)
{
    float angleInDeg;
    float radToDeg = 57.2957795131;
    if (x2 - x1 == 0)
    {
        angleInDeg = 90;
    }
    else
    {
        angleInDeg = floatingPointModulo(180.0 + radToDeg*std::atan((y2 - y1) / (x2 - x1)), 180);
    }
    
    if (radians) return angleInDeg/radToDeg;
    else return angleInDeg;
}

bool Line::intersectsLine(Line *line)
{
}