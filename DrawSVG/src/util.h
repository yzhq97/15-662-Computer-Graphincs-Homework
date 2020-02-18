#ifndef DRAWSVG_UTIL_H
#define DRAWSVG_UTIL_H

#include <cstdio>
#include <iostream>
#include "CMU462.h"

namespace util {

  const float eps = 1e-9;

  inline uint8_t float_to_uint8 (float x) {
    return (uint8_t) ( 255.f * std::max( 0.0f, std::min( 1.0f, x)));
  }

  float cross_product (float x, float y, float x0, float y0, float x1, float y1);
  bool point_in_triangle (float x, float y, float x0, float y0, float x1, float y1, float x2, float y2);

  void supersample (uint8_t* canvas, float* buffer, int canvas_w, int canvas_h, int sample_rate);
  void resample (float* buffer, uint8_t* canvas, int canvas_w, int canvas_h, int sample_rate);
  void resample (float* buffer, float* canvas, int canvas_w, int canvas_h, int sample_rate);

  void premultiply_alpha(float* source, float* target, int w, int h);
  void depremultiply_alpha(float* source, float* target, int w, int h);
  void alpha_blend_pixel (float* target, int w, int h, int x, int y, float Er, float Eg, float Eb, float Ea);

}

#endif //DRAWSVG_UTIL_H
