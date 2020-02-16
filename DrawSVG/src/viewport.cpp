#include "viewport.h"

#include "CMU462.h"

namespace CMU462 {

void ViewportImp::set_viewbox( float centerX, float centerY, float vspan ) {

  // Task 5 (part 2): 
  // Set svg coordinate to normalized device coordinate transformation. Your input
  // arguments are defined as normalized SVG canvas coordinates.
  this->centerX = centerX;
  this->centerY = centerY;
  this->vspan = vspan;

  float s = 1.0f / (2.0f * vspan);

  Matrix3x3 x;
  x(0, 0) = s;  x(0, 1) = 0.; x(0, 2) = 0.5 - s * centerX;
  x(1, 0) = 0.; x(1, 1) = s;  x(1, 2) = 0.5 - s * centerY;
  x(2, 0) = 0.; x(2, 1) = 0.; x(2, 2) = 1.;

  set_svg_2_norm(x);

}

void ViewportImp::update_viewbox( float dx, float dy, float scale ) {

  this->centerX -= dx;
  this->centerY -= dy;
  this->vspan *= scale;

  set_viewbox( centerX, centerY, vspan );
}

} // namespace CMU462
