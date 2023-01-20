#pragma once

class Line {
public:
    Line(int x1, int y1, int x2, int y2);
    Line(float x1, float y1, float x2, float y2);

    float getSlope();
    float getAngleFromPosXAxis(bool radians = false);
    bool intersectsLine(Line *line);

private:
    float x1, y1;
    float x2, y2;
};