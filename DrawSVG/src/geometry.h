#ifndef DRAWSVG_GEOMETRY_H
#define DRAWSVG_GEOMETRY_H

#include <stdio.h>

namespace ZQ {
    int point_on_line (float x, float y, float x0, float y0, float x1, float y1);
    bool point_in_triangle (float x, float y, float x0, float y0, float x1, float y1, float x2, float y2);
}

#endif //DRAWSVG_GEOMETRY_H
