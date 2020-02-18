#include "util.h"

using namespace std;

namespace util {

  float cross_product (float x, float y, float x0, float y0, float x1, float y1) {
    float v0_x = x1 - x0, v0_y = y1 - y0;
    float v1_x = x - x1, v1_y = y - y1;
    return v0_x * v1_y - v1_x * v0_y;
  }

  bool point_in_triangle (float x, float y, float x0, float y0, float x1, float y1, float x2, float y2) {
    float cp = cross_product(x0, y0, x1, y1, x2, y2);
    float cp0 = cross_product(x, y, x1, y1, x2, y2) / cp;
    float cp1 = cross_product(x, y, x2, y2, x0, y0) / cp;
    float cp2 = cross_product(x, y, x0, y0, x1, y1) / cp;
    return cp0 > -eps && cp1 > -eps && cp2 > -eps;
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

  void supersample (uint8_t* canvas, float* buffer, int canvas_w, int canvas_h, int sample_rate) {
    int xs, ys, xt, yt;
    int buffer_w = canvas_w * sample_rate, buffer_h = canvas_h * sample_rate;
    for (xt = 0; xt < buffer_w; xt++) for (yt = 0; yt < buffer_h; yt++) {
        xs = xt / sample_rate; ys = yt / sample_rate;
        buffer[4 * (xt + yt * buffer_w)    ] = canvas[4 * (xs + ys * canvas_w)    ] / 255.0f;
        buffer[4 * (xt + yt * buffer_w) + 1] = canvas[4 * (xs + ys * canvas_w) + 1] / 255.0f;
        buffer[4 * (xt + yt * buffer_w) + 2] = canvas[4 * (xs + ys * canvas_w) + 2] / 255.0f;
        buffer[4 * (xt + yt * buffer_w) + 3] = canvas[4 * (xs + ys * canvas_w) + 3] / 255.0f;
      }
  }

  void resample (float* buffer, uint8_t* canvas, int canvas_w, int canvas_h, int sample_rate) {
    int xb, yb, xc, yc;
    float r, g, b, a;
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
        canvas[4 * (xc + yc * canvas_w)    ] = float_to_uint8(r / (sample_rate * sample_rate));
        canvas[4 * (xc + yc * canvas_w) + 1] = float_to_uint8(g / (sample_rate * sample_rate));
        canvas[4 * (xc + yc * canvas_w) + 2] = float_to_uint8(b / (sample_rate * sample_rate));
        canvas[4 * (xc + yc * canvas_w) + 3] = float_to_uint8(a / (sample_rate * sample_rate));
      }
  }

  void resample (float* buffer, float* canvas, int canvas_w, int canvas_h, int sample_rate) {
    int xb, yb, xc, yc;
    float r, g, b, a;
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
        canvas[4 * (xc + yc * canvas_w)    ] = r;
        canvas[4 * (xc + yc * canvas_w) + 1] = g;
        canvas[4 * (xc + yc * canvas_w) + 2] = b;
        canvas[4 * (xc + yc * canvas_w) + 3] = a;
      }
  }

  void premultiply_alpha(float* source, float* target, int w, int h) {
    for (int x = 0; x < w; x++) for (int y = 0; y < h; y++) {
      target[4 * (x + y * w)    ] = source[4 * (x + y * w)    ] * source[4 * (x + y * w) + 3];
      target[4 * (x + y * w) + 1] = source[4 * (x + y * w) + 1] * source[4 * (x + y * w) + 3];
      target[4 * (x + y * w) + 2] = source[4 * (x + y * w) + 2] * source[4 * (x + y * w) + 3];
      target[4 * (x + y * w) + 3] = source[4 * (x + y * w) + 3];
    }
  }

  void depremultiply_alpha(float* source, float* target, int w, int h) {
    float a;
    for (int x = 0; x < w; x++) for (int y = 0; y < h; y++) {
      a = source[4 * (x + y * w) + 3] + eps;
      target[4 * (x + y * w)    ] = source[4 * (x + y * w)    ] / a;
      target[4 * (x + y * w) + 1] = source[4 * (x + y * w) + 1] / a;
      target[4 * (x + y * w) + 2] = source[4 * (x + y * w) + 2] / a;
      target[4 * (x + y * w) + 3] = source[4 * (x + y * w) + 3];
    }
  }

  void alpha_blend_pixel (float* target, int w, int h, int x, int y, float Er, float Eg, float Eb, float Ea) {
    Er *= Ea; Eg *= Ea; Eb *= Ea;
    float Ca = target[4 * (x + y * w) + 3];
    float Cr = target[4 * (x + y * w)    ];
    float Cg = target[4 * (x + y * w) + 1];
    float Cb = target[4 * (x + y * w) + 2];
    target[4 * (x + y * w)    ] = Er + (1.0f - Ea) * Cr;
    target[4 * (x + y * w) + 1] = Eg + (1.0f - Ea) * Cg;
    target[4 * (x + y * w) + 2] = Eb + (1.0f - Ea) * Cb;
    target[4 * (x + y * w) + 3] = Ea + (1.0f - Ea) * Ca;
  }
}