#pragma once

class Line {
    Line(int x1, int y1, int x2, int y2);

    float getSlope();
    float getAngleFromPosXAxis(bool radians = false);
    bool intersectsLine(Line *line);

private:
    int x1, y1;
    int x2, y2;
};