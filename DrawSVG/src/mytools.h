#ifndef DRAWSVG_MYTOOLS_H
#define DRAWSVG_MYTOOLS_H

#include <cstdio>
#include <iostream>

namespace ZQ {

  const float eps = 1e-9;

  float cross_product (float x, float y, float x0, float y0, float x1, float y1);

  bool point_in_triangle (float x, float y, float x0, float y0, float x1, float y1, float x2, float y2);

  void downsample (float* source, float* target, int target_w, int target_h, int sample_rate);

  void alpha_blend_pixel (uint8_t* target, int target_w, int target_h, int x, int y, float Er, float Eg, float Eb, float Ea);
}

#endif //DRAWSVG_MYTOOLS_H
