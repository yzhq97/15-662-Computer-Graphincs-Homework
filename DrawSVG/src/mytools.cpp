#include "mytools.h"
#include <cmath>
#include <bits/stdc++.h>

using namespace std;

namespace ZQ {

  PointLineRel point_line_test (float x, float y, float x0, float y0, float x1, float y1) {

    float v0_x = x1 - x0, v0_y = y1 - y0;
    float v1_x = x - x1, v1_y = y - y1;
    float cross_product = v0_x * v1_y - v1_x * v0_y;

    if (cross_product > eps) {
      return LEFT; // point is on the left of vector
    } else if (cross_product < -eps) {
      return RIGHT; // point is on the right of vector
    } else {
      float v2_x = x - x0, v2_y = y - y0;
      float n0_squared = v0_x * v0_x + v0_y * v0_y;
      float n2_squared = v2_x * v2_x + v2_y * v2_y;
      float squared_ratio = n2_squared / n0_squared;
      if (squared_ratio < -eps) {
        return BEHIND;  // point is behind vector
      } else if (squared_ratio >= 1.0f-eps) {
        return AHEAD;  // point is ahead of vector
      } else {
        return ON;  // point is on the vector
      }
    }

  }

  bool point_in_triangle (float x, float y, float x0, float y0, float x1, float y1, float x2, float y2) {

    // sort the three vertices (primary key: y, secondary key: x)
    float xs0=x0, ys0=y0, xs1=x1, ys1=y1, xs2=x2, ys2=y2;
    if (ys1 < ys0) { swap(xs0, xs1); swap(ys0, ys1); }
    if (ys2 < ys0) { swap(xs0, xs2); swap(ys0, ys2); }
    if (ys1 < ys2) { swap(xs1, xs2); swap(ys1, ys2); }
    if (ys0 == ys1 && xs1 < xs0) { swap(xs0, xs1); swap(ys0, ys1); }
    if (ys1 == ys2 && xs2 < xs1) { swap(xs1, xs2); swap(ys1, ys2); }

    // do three point-line relationship tests
    PointLineRel rel01 = point_line_test(x, y, xs0, ys0, xs1, ys1);
    PointLineRel rel12 = point_line_test(x, y, xs1, ys1, xs2, ys2);
    PointLineRel rel20 = point_line_test(x, y, xs2, ys2, xs0, ys0);

    // see if the point is in the triangle
    if (rel01 == LEFT && rel12 == LEFT && rel20 == LEFT) return true;
    if (rel01 == RIGHT || rel12 == RIGHT || rel20 == RIGHT) return false;

    // edge01 is always bottom/right edge, so use default behavior

    // edge12 could be left/top/right edge
    if (rel12 == ON && xs1 < xs0) return true; // xs1 < xs0 means edge12 is top/left edge

    // edge20 is always left edge
    if (rel20 == ON) return true;

    // default behavior
    return false;
  }

  void downsample (uint8_t* source, uint8_t* target, int target_w, int target_h, int sample_rate) {
    return;
  }

  void alpha_blend_pixel (uint8_t* target, int target_w, int target_h, int x, int y, CMU462::Color color) {
    float Cr = target[4 * (x + y * target_w)    ] / 255.0f;
    float Cg = target[4 * (x + y * target_w) + 1] / 255.0f;
    float Cb = target[4 * (x + y * target_w) + 2] / 255.0f;
    float Ca = target[4 * (x + y * target_w) + 3] / 255.0f;
    float r = color.r + (1.0f - color.a) * Cr;
    float g = color.g + (1.0f - color.a) * Cg;
    float b = color.b + (1.0f - color.a) * Cb;
    float a = 1.0f - (1.0f - color.a) * (1.0f - Ca);
    target[4 * (x + y * target_w)    ] = (uint8_t) (255.0f * r);
    target[4 * (x + y * target_w) + 1] = (uint8_t) (255.0f * g);
    target[4 * (x + y * target_w) + 2] = (uint8_t) (255.0f * b);
    target[4 * (x + y * target_w) + 3] = (uint8_t) (255.0f * a);
  }
}