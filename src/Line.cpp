#include "Line.hpp"

#include <cmath>

#include "common.hpp"
#include "Game.hpp"

Line::Line(int x1, int y1, int x2, int y2)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;

    if (x1 == x2 && y1 == y2)
    {
        Game::add_error("Line is not a line.");
    }
}

Line::Line(float x1, float y1, float x2, float y2)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;

    if (x1 == x2 && y1 == y2)
    {
        Game::add_error("Line is not a line.");
    }
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
        angleInDeg = floatingPointModulo(180.0 + radToDeg * std::atan((y2 - y1) / (x2 - x1)), 180);
    }

    float angle = angleInDeg;
    if (radians)
        angle /= radToDeg;

    return angle;
}

bool Line::intersectsLine(Line *line)
{
    // line.x1
    int o1 = orientation(this->x1, this->y1, this->x2, this->y2, line->x1, line->y1);
    int o2 = orientation(this->x1, this->y1, this->x2, this->y2, line->x2, line->y2);
    int o3 = orientation(line->x1, line->y1, line->x2, line->y2, this->x1, this->y1);
    int o4 = orientation(line->x1, line->y1, line->x2, line->y2, this->x2, this->y2);

    bool notColinear = o1 != o2 && o3 != o4;
    if (notColinear)
    {
        return true;
    }

    // p1: this->x1, this->y1
    // p2: line->x1, line->y1
    // q1: this->x2, this->y2
    // q2: line->x2, line->y2

    // Special Cases
    // p1, q1 and p2 are collinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(this->x1, this->y1, line->x1, line->y1, this->x2, this->y2)) return true;
    // p1, q1 and q2 are collinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(this->x1, this->y1, line->x2, line->y2, this->x2, this->y2)) return true;
    // p2, q2 and p1 are collinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(line->x1, line->y1, this->x1, this->y1, line->x2, line->y2)) return true;
    // p2, q2 and q1 are collinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(line->x1, line->y1, this->x2, this->y2, line->x2, line->y2)) return true;

    return false;
}