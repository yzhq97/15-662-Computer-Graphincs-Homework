#ifndef DRAWSVG_MYTOOLS_H
#define DRAWSVG_MYTOOLS_H

#include <cstdio>
#include <iostream>
#include "color.h"

namespace ZQ {

  const float eps = 1e-9;

  typedef enum PointLineRel{
    ON,
    BEHIND,
    AHEAD,
    LEFT,
    RIGHT
  } PointLineRel;

  PointLineRel point_line_test (float x, float y, float x0, float y0, float x1, float y1);

  bool point_in_triangle (float x, float y, float x0, float y0, float x1, float y1, float x2, float y2);

  void downsample (uint8_t* source, uint8_t* target, int target_w, int target_h, int sample_rate);

  void alpha_blend_pixel (uint8_t* target, int target_w, int target_h, int x, int y, CMU462::Color color);
}

#endif //DRAWSVG_MYTOOLS_H
