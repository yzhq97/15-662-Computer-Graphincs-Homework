#include "texture.h"
#include "color.h"

#include <assert.h>
#include <iostream>
#include <algorithm>

#include "util.h"

using namespace std;

namespace CMU462 {

inline void uint8_to_float( float dst[4], unsigned char* src ) {
  uint8_t* src_uint8 = (uint8_t *)src;
  dst[0] = src_uint8[0] / 255.f;
  dst[1] = src_uint8[1] / 255.f;
  dst[2] = src_uint8[2] / 255.f;
  dst[3] = src_uint8[3] / 255.f;
}

inline void float_to_uint8( unsigned char* dst, float src[4] ) {
  uint8_t* dst_uint8 = (uint8_t *)dst;
  dst_uint8[0] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[0])));
  dst_uint8[1] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[1])));
  dst_uint8[2] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[2])));
  dst_uint8[3] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[3])));
}

void Sampler2DImp::generate_mips(Texture& tex, int startLevel) {

  // NOTE: 
  // This starter code allocates the mip levels and generates a level 
  // map by filling each level with placeholder data in the form of a 
  // color that differs from its neighbours'. You should instead fill
  // with the correct data!

  // Task 7: Implement this

  // check start level
  if ( startLevel >= tex.mipmap.size() ) {
    std::cerr << "Invalid start level"; 
  }

  // allocate sublevels
  int baseWidth  = tex.mipmap[startLevel].width;
  int baseHeight = tex.mipmap[startLevel].height;
  int numSubLevels = (int)(log2f( (float)max(baseWidth, baseHeight)));

  numSubLevels = min(numSubLevels, kMaxMipLevels - startLevel - 1);
  tex.mipmap.resize(startLevel + numSubLevels + 1);

  int width  = baseWidth;
  int height = baseHeight;
  for (int i = 1; i <= numSubLevels; i++) {

    MipLevel& level = tex.mipmap[startLevel + i];

    // handle odd size texture by rounding down
    width  = max( 1, width  / 2); assert(width  > 0);
    height = max( 1, height / 2); assert(height > 0);

    level.width = width;
    level.height = height;
    level.texels = vector<unsigned char>(4 * width * height);

  }

  for(size_t i = 1; i < tex.mipmap.size(); ++i) {
    MipLevel& mip = tex.mipmap[i];
    MipLevel& mips = tex.mipmap[i-1];
    int us, vs;
    float r, g, b, a;
    for (int u = 0; u < mip.width; u++) for (int v = 0; v < mip.height; v++) {
      r = g = b = a = 0.0f;
      for (int dx = 0; dx < 2; dx++) for (int dy = 0; dy < 2; dy++) {
          us = 2 * u + dx; vs = 2 * v + dy;
          r += mips.texels[4 * (us + vs * mips.width)    ] / 255.0f;
          g += mips.texels[4 * (us + vs * mips.width) + 1] / 255.0f;
          b += mips.texels[4 * (us + vs * mips.width) + 2] / 255.0f;
          a += mips.texels[4 * (us + vs * mips.width) + 3] / 255.0f;
      }
      mip.texels[4 * (u + v * mip.width)    ] = util::float_to_uint8(r/4.0f);
      mip.texels[4 * (u + v * mip.width) + 1] = util::float_to_uint8(g/4.0f);
      mip.texels[4 * (u + v * mip.width) + 2] = util::float_to_uint8(b/4.0f);
      mip.texels[4 * (u + v * mip.width) + 3] = util::float_to_uint8(a/4.0f);
    }
  }

}

Color Sampler2DImp::sample_nearest(Texture& tex, 
                                   float u, float v, 
                                   int level) {

  // Task 6: Implement nearest neighbour interpolation
  int ui = int(u * tex.mipmap[level].width);
  int vi = int(v * tex.mipmap[level].height);
  float r = tex.mipmap[level].texels[4 * (ui + vi * tex.mipmap[level].width)    ];
  float g = tex.mipmap[level].texels[4 * (ui + vi * tex.mipmap[level].width) + 1];
  float b = tex.mipmap[level].texels[4 * (ui + vi * tex.mipmap[level].width) + 2];
  float a = tex.mipmap[level].texels[4 * (ui + vi * tex.mipmap[level].width) + 3];
  return {r, g, b, a};

}

Color Sampler2DImp::sample_bilinear(Texture& tex,
                                    float u, float v,
                                    int level) {

  // Task 6: Implement bilinear filtering
  auto & texels = tex.mipmap[level].texels;
  int width = tex.mipmap[level].width, height = tex.mipmap[level].height;

  float ut = u * width, vt = v * height;
  int u0 = (int)floor(ut), v0 = (int)floor(vt);
  int u1 = (int)ceil(ut), v1 = (int)ceil(vt);
  u1 = min(u1, width - 1); v1 = min(v1, height - 1);
  float ou = ut - u0, ov = vt - v0;

  float r00 = texels[4 * (u0 + v0 * width)    ] / 255.0f;
  float g00 = texels[4 * (u0 + v0 * width) + 1] / 255.0f;
  float b00 = texels[4 * (u0 + v0 * width) + 2] / 255.0f;
  float a00 = texels[4 * (u0 + v0 * width) + 3] / 255.0f;
  Color c00 = Color(r00, g00, b00, a00);

  float r10 = texels[4 * (u0 + v1 * width)    ] / 255.0f;
  float g10 = texels[4 * (u0 + v1 * width) + 1] / 255.0f;
  float b10 = texels[4 * (u0 + v1 * width) + 2] / 255.0f;
  float a10 = texels[4 * (u0 + v1 * width) + 3] / 255.0f;
  Color c10 = Color(r10, g10, b10, a10);

  float r01 = texels[4 * (u1 + v0 * width)    ] / 255.0f;
  float g01 = texels[4 * (u1 + v0 * width) + 1] / 255.0f;
  float b01 = texels[4 * (u1 + v0 * width) + 2] / 255.0f;
  float a01 = texels[4 * (u1 + v0 * width) + 3] / 255.0f;
  Color c01 = Color(r01, g01, b01, a01);

  float r11 = texels[4 * (u1 + v1 * width)    ] / 255.0f;
  float g11 = texels[4 * (u1 + v1 * width) + 1] / 255.0f;
  float b11 = texels[4 * (u1 + v1 * width) + 2] / 255.0f;
  float a11 = texels[4 * (u1 + v1 * width) + 3] / 255.0f;
  Color c11 = Color(r11, g11, b11, a11);

  Color c0 = (1.0f - ou) * c00 + ou * c01;
  Color c1 = (1.0f - ou) * c10 + ou * c11;

  Color c = (1.0f - ov) * c0 + ov * c1;

  return c;

}

Color Sampler2DImp::sample_trilinear(Texture& tex, 
                                     float u, float v, 
                                     float u_scale, float v_scale) {

  // Task 7: Implement trilinear filtering
  // u_scale: box_width * sample_rate, v_scale: box_height * sample_rate
  float scale = sqrt(u_scale * v_scale);
  float level = 0.5 * log2((1.0f * tex.mipmap[0].width * tex.mipmap[0].height) / (u_scale * v_scale));
  if (level <= 0) return sample_bilinear(tex, u, v, 0);

  int level0 = (int)floor(level), level1 = (int)ceil(level);
  Color c0 = sample_bilinear(tex, u, v, level0);
  Color c1 = sample_bilinear(tex, u, v, level1);

  float scale0 = sqrt(1.0f * tex.mipmap[level0].width * tex.mipmap[level0].height);
  float scale1 = sqrt(1.0f * tex.mipmap[level1].width * tex.mipmap[level1].height);
  float ol = (scale - scale0) / (scale1 - scale0);

  return (1.0f - ol) * c0 + ol * c1;

}

} // namespace CMU462
