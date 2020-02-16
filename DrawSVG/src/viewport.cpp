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

//  Matrix3x3 B;
//
//  B(0,0) = 0.; B(0,1) = 0.; B(0,2) = 0.;
//  B(1,0) = 0.; B(1,1) = 0.; B(1,2) = 0.;
//  B(2,0) = 0.; B(2,1) = 0.; B(2,2) = 0.;
//
//  set_svg_2_norm(B);

  Matrix3x3 x = svg_2_norm;
  printf("\n");
  printf("%.3f %.3f %.3f\n", centerX, centerY, vspan);
  printf("%.3f %.3f %.3f\n", x(0, 0), x(0, 1), x(0, 2));
  printf("%.3f %.3f %.3f\n", x(1, 0), x(1, 1), x(1, 2));
  printf("%.3f %.3f %.3f\n", x(2, 0), x(2, 1), x(2, 2));

  this->centerX -= dx;
  this->centerY -= dy;
  this->vspan *= scale;
}

void ViewportImp::update_viewbox( float dx, float dy, float scale ) { 
  

  set_viewbox( dx, dy, scale );
}

} // namespace CMU462
