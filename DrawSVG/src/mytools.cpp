#include "mytools.h"
#include <cmath>

using namespace std;

namespace ZQ {

  float cross_product (float x, float y, float x0, float y0, float x1, float y1) {
    float v0_x = x1 - x0, v0_y = y1 - y0;
    float v1_x = x - x1, v1_y = y - y1;
    return v0_x * v1_y - v1_x * v0_y;
  }

  bool point_in_triangle (float x, float y, float x0, float y0, float x1, float y1, float x2, float y2) {
    float cp0 = cross_product(x, y, x1, y1, x2, y2);
    float cp1 = cross_product(x, y, x2, y2, x0, y0);
    float cp2 = cross_product(x, y, x0, y0, x1, y1);
    return cp0 >= 0.0f && cp1 >= 0.0f && cp2 >= 0.0f;
  }

  void downsample (float* source, float* target, int target_w, int target_h, int sample_rate) {
    int sx, sy;
    float r, g, b, a;
    int super_w = target_w * sample_rate, super_h = target_h * sample_rate;
    for (int x = 0; x < target_w; x++) for (int y=0; y < target_h; y++) {
      r = g = b = a = 0.0f;
      for (int dx = 0; dx < sample_rate; dx++) for (int dy = 0; dy < sample_rate; dy++) {
        sx = x * sample_rate + dx;
        sy = y * sample_rate + dy;
        r += source[4 * (sx + sy * super_w)    ];
        g += source[4 * (sx + sy * super_w) + 1];
        b += source[4 * (sx + sy * super_w) + 2];
        a += source[4 * (sx + sy * super_w) + 3];
      }
      target[4 * (x + y * target_w)    ] = r / (sample_rate * sample_rate);
      target[4 * (x + y * target_w) + 1] = g / (sample_rate * sample_rate);
      target[4 * (x + y * target_w) + 2] = b / (sample_rate * sample_rate);
      target[4 * (x + y * target_w) + 3] = a / (sample_rate * sample_rate);
    }
  }

  void alpha_blend_pixel (uint8_t* target, int target_w, int target_h, int x, int y, float Er, float Eg, float Eb, float Ea) {
    float Cr = target[4 * (x + y * target_w)    ] / 255.0f;
    float Cg = target[4 * (x + y * target_w) + 1] / 255.0f;
    float Cb = target[4 * (x + y * target_w) + 2] / 255.0f;
    float Ca = target[4 * (x + y * target_w) + 3] / 255.0f;
    float r = Ea * Er + (1.0f - Ea) * Cr;
    float g = Ea * Eg + (1.0f - Ea) * Cg;
    float b = Ea * Eb + (1.0f - Ea) * Cb;
    float a = 1.0f - (1.0f - Ea) * (1.0f - Ca);
    target[4 * (x + y * target_w)    ] = (uint8_t) (255.0f * r);
    target[4 * (x + y * target_w) + 1] = (uint8_t) (255.0f * g);
    target[4 * (x + y * target_w) + 2] = (uint8_t) (255.0f * b);
    target[4 * (x + y * target_w) + 3] = (uint8_t) (255.0f * a);
  }
}