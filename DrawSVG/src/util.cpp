#include "util.h"
#include <cmath>

using namespace std;

namespace util {

  float cross_product (float x, float y, float x0, float y0, float x1, float y1) {
    float v0_x = x1 - x0, v0_y = y1 - y0;
    float v1_x = x - x1, v1_y = y - y1;
    return v0_x * v1_y - v1_x * v0_y;
  }

  bool point_in_triangle (float x, float y, float x0, float y0, float x1, float y1, float x2, float y2) {
    float cp0 = cross_product(x, y, x1, y1, x2, y2);
    float cp1 = cross_product(x, y, x2, y2, x0, y0);
    float cp2 = cross_product(x, y, x0, y0, x1, y1);
    if (cp0 > -eps && cp1 > -eps && cp2 > -eps) return true;
    if (cp0 < eps && cp1 < eps && cp2 < eps) return true;
    return false;
  }

  void supersample (uint8_t* canvas, uint8_t* buffer, int canvas_w, int canvas_h, int sample_rate) {
    int xs, ys, xt, yt;
    int buffer_w = canvas_w * sample_rate, buffer_h = canvas_h * sample_rate;
    for (xt = 0; xt < buffer_w; xt++) for (yt = 0; yt < buffer_h; yt++) {
      xs = xt / sample_rate; ys = yt / sample_rate;
        buffer[4 * (xt + yt * buffer_w)    ] = canvas[4 * (xs + ys * canvas_w)    ];
        buffer[4 * (xt + yt * buffer_w) + 1] = canvas[4 * (xs + ys * canvas_w) + 1];
        buffer[4 * (xt + yt * buffer_w) + 2] = canvas[4 * (xs + ys * canvas_w) + 2];
        buffer[4 * (xt + yt * buffer_w) + 3] = canvas[4 * (xs + ys * canvas_w) + 3];
    }
  }

  void resample (uint8_t* buffer, uint8_t* canvas, int canvas_w, int canvas_h, int sample_rate) {
    int xb, yb, xc, yc;
    int r, g, b, a;
    int buffer_w = canvas_w * sample_rate;
    for (xc = 0; xc < canvas_w; xc++) for (yc = 0; yc < canvas_h; yc++) {
      r = g = b = a = 0;
      for (int dx = 0; dx < sample_rate; dx++) for (int dy = 0; dy < sample_rate; dy++) {
          xb = xc * sample_rate + dx; yb = yc * sample_rate + dy;
          r += buffer[4 * (xb + yb * buffer_w)    ];
          g += buffer[4 * (xb + yb * buffer_w) + 1];
          b += buffer[4 * (xb + yb * buffer_w) + 2];
          a += buffer[4 * (xb + yb * buffer_w) + 3];
      }
      canvas[4 * (xc + yc * canvas_w)    ] = (uint8_t) (r / (sample_rate * sample_rate));
      canvas[4 * (xc + yc * canvas_w) + 1] = (uint8_t) (g / (sample_rate * sample_rate));
      canvas[4 * (xc + yc * canvas_w) + 2] = (uint8_t) (b / (sample_rate * sample_rate));
      canvas[4 * (xc + yc * canvas_w) + 3] = (uint8_t) (a / (sample_rate * sample_rate));
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