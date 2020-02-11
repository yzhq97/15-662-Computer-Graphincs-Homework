#include "software_renderer.h"

#include <cstdio>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

#include "triangulation.h"
#include "mytools.h"

using namespace std;

namespace CMU462 {


// Implements SoftwareRenderer //

void SoftwareRendererImp::draw_svg( SVG& svg ) {

  // set top level transformation
  transformation = svg_2_screen;

  // draw all elements
  for ( size_t i = 0; i < svg.elements.size(); ++i ) {
    draw_element(svg.elements[i]);
  }

  // draw canvas outline
  Vector2D a = transform(Vector2D(    0    ,     0    )); a.x--; a.y--;
  Vector2D b = transform(Vector2D(svg.width,     0    )); b.x++; b.y--;
  Vector2D c = transform(Vector2D(    0    ,svg.height)); c.x--; c.y++;
  Vector2D d = transform(Vector2D(svg.width,svg.height)); d.x++; d.y++;

  rasterize_line(a.x, a.y, b.x, b.y, Color::Black);
  rasterize_line(a.x, a.y, c.x, c.y, Color::Black);
  rasterize_line(d.x, d.y, b.x, b.y, Color::Black);
  rasterize_line(d.x, d.y, c.x, c.y, Color::Black);

  // resolve and send to render target
  resolve();

}

void SoftwareRendererImp::set_sample_rate( size_t sample_rate ) {

  // Task 4: 
  // You may want to modify this for supersampling support
  this->sample_rate = sample_rate;

}

void SoftwareRendererImp::set_render_target( unsigned char* render_target,
                                             size_t width, size_t height ) {

  // Task 4: 
  // You may want to modify this for supersampling support
  this->render_target = render_target;
  this->target_w = width;
  this->target_h = height;

}

void SoftwareRendererImp::draw_element( SVGElement* element ) {

  // Task 5 (part 1):
  // Modify this to implement the transformation stack

  switch(element->type) {
    case POINT:
      draw_point(static_cast<Point&>(*element));
      break;
    case LINE:
      draw_line(static_cast<Line&>(*element));
      break;
    case POLYLINE:
      draw_polyline(static_cast<Polyline&>(*element));
      break;
    case RECT:
      draw_rect(static_cast<Rect&>(*element));
      break;
    case POLYGON:
      draw_polygon(static_cast<Polygon&>(*element));
      break;
    case ELLIPSE:
      draw_ellipse(static_cast<Ellipse&>(*element));
      break;
    case IMAGE:
      draw_image(static_cast<Image&>(*element));
      break;
    case GROUP:
      draw_group(static_cast<Group&>(*element));
      break;
    default:
      break;
  }

}


// Primitive Drawing //

void SoftwareRendererImp::draw_point( Point& point ) {

  Vector2D p = transform(point.position);
  rasterize_point( p.x, p.y, point.style.fillColor );

}

void SoftwareRendererImp::draw_line( Line& line ) { 

  Vector2D p0 = transform(line.from);
  Vector2D p1 = transform(line.to);
  rasterize_line( p0.x, p0.y, p1.x, p1.y, line.style.strokeColor );

}

void SoftwareRendererImp::draw_polyline( Polyline& polyline ) {

  Color c = polyline.style.strokeColor;

  if( c.a != 0 ) {
    int nPoints = polyline.points.size();
    for( int i = 0; i < nPoints - 1; i++ ) {
      Vector2D p0 = transform(polyline.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polyline.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_rect( Rect& rect ) {

  Color c;
  
  // draw as two triangles
  float x = rect.position.x;
  float y = rect.position.y;
  float w = rect.dimension.x;
  float h = rect.dimension.y;

  Vector2D p0 = transform(Vector2D(   x   ,   y   ));
  Vector2D p1 = transform(Vector2D( x + w ,   y   ));
  Vector2D p2 = transform(Vector2D(   x   , y + h ));
  Vector2D p3 = transform(Vector2D( x + w , y + h ));
  
  // draw fill
  c = rect.style.fillColor;
  if (c.a != 0 ) {
    rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    rasterize_triangle( p2.x, p2.y, p1.x, p1.y, p3.x, p3.y, c );
  }

  // draw outline
  c = rect.style.strokeColor;
  if( c.a != 0 ) {
    rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    rasterize_line( p1.x, p1.y, p3.x, p3.y, c );
    rasterize_line( p3.x, p3.y, p2.x, p2.y, c );
    rasterize_line( p2.x, p2.y, p0.x, p0.y, c );
  }

}

void SoftwareRendererImp::draw_polygon( Polygon& polygon ) {

  Color c;

  // draw fill
  c = polygon.style.fillColor;
  if( c.a != 0 ) {

    // triangulate
    vector<Vector2D> triangles;
    triangulate( polygon, triangles );

    // draw as triangles
    for (size_t i = 0; i < triangles.size(); i += 3) {
      Vector2D p0 = transform(triangles[i + 0]);
      Vector2D p1 = transform(triangles[i + 1]);
      Vector2D p2 = transform(triangles[i + 2]);
      rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    }
  }

  // draw outline
  c = polygon.style.strokeColor;
  if( c.a != 0 ) {
    int nPoints = polygon.points.size();
    for( int i = 0; i < nPoints; i++ ) {
      Vector2D p0 = transform(polygon.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polygon.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_ellipse( Ellipse& ellipse ) {

  // Extra credit 

}

void SoftwareRendererImp::draw_image( Image& image ) {

  Vector2D p0 = transform(image.position);
  Vector2D p1 = transform(image.position + image.dimension);

  rasterize_image( p0.x, p0.y, p1.x, p1.y, image.tex );
}

void SoftwareRendererImp::draw_group( Group& group ) {

  for ( size_t i = 0; i < group.elements.size(); ++i ) {
    draw_element(group.elements[i]);
  }

}

// Rasterization //

// The input arguments in the rasterization functions 
// below are all defined in screen space coordinates

void SoftwareRendererImp::rasterize_point( float x, float y, Color color ) {

  // fill in the nearest pixel
  int sx = (int) floor(x);
  int sy = (int) floor(y);

  // check bounds
  if ( sx < 0 || sx >= target_w ) return;
  if ( sy < 0 || sy >= target_h ) return;

  // fill sample - with alpha blending!
  ZQ::alpha_blend_pixel(render_target, target_w, target_h, sx, sy, color.r, color.g, color.b, color.a);

}

void SoftwareRendererImp::rasterize_line( float x0, float y0,
                                          float x1, float y1,
                                          Color color) {

  // Task 2: 
  // Implement line rasterization
  float linewidth = 1.0f;

  // Convert the line to a rectangle
  float vx = x1 - x0, vy = y1 - y0;
  float norm = sqrt(vx * vx + vy * vy);
  float ox = -vy / norm, oy = vx / norm; // orthogonal direction
  float dx = 0.5f * linewidth * ox, dy = 0.5f * linewidth * oy;

  float tx0 = x0 + dx, ty0 = y0 + dy;
  float tx1 = x0 - dx, ty1 = y0 - dy;
  float tx2 = x1 - dx, ty2 = y1 - dy;
  float tx3 = x1 + dx, ty3 = y1 + dy;

  rasterize_triangle(tx0, ty0, tx1, ty1, tx3, ty3, color);
  rasterize_triangle(tx1, ty1, tx2, ty2, tx3, ty3, color);
}

void SoftwareRendererImp::rasterize_triangle( float x0, float y0,
                                              float x1, float y1,
                                              float x2, float y2,
                                              Color color ) {
  // Task 3: 
  // Implement triangle rasterization
//  printf("rasterize_triangle called with (%.2f, %.2f), (%.2f, %.2f), (%.2f, %.2f) ", x0, y0, x1, y1, x2, y2);
//  printf("on %d x %d canvas with sample rate %d\n", target_w, target_h, sample_rate);

  // get bounding box
  int x_low = (int)floor(min({x0, x1, x2}));
  int x_high = (int)ceil(max({x0, x1, x2}));
  int y_low = (int)floor(min({y0, y1, y2}));
  int y_high = (int)ceil(max({y0, y1, y2}));
  if (x_low < 0) x_low = 0; if (x_high >= target_w) x_high = target_w-1;
  if (y_low < 0) y_low = 0; if (y_high >= target_h) y_high = target_h-1;
  int xs_low = x_low * sample_rate, xs_high = x_high * sample_rate;
  int ys_low = y_low * sample_rate, ys_high = y_high * sample_rate;

  int box_w = x_high - x_low, box_h = y_high - y_low;
  int super_w = box_w * sample_rate, super_h = box_h * sample_rate;
//  printf("\tusing a %d x %d bounding box\n", box_w, box_h);

  // make supersampling buffer
  auto* buffer = new float[super_w * super_h * 4];
  auto* downsampled = new float[box_w * box_h * 4];
  fill_n(buffer, super_w * super_h * 4, 0.0f);
//  printf("\tsupersampling buffer has been made\n");

  // get triangle vertices in supersampling space
  float xs0 = x0 * sample_rate, ys0 = y0 * sample_rate;
  float xs1 = x1 * sample_rate, ys1 = y1 * sample_rate;
  float xs2 = x2 * sample_rate, ys2 = y2 * sample_rate;

  // iterate and do point in triangle test
  for (int x = 0; x <= super_w; x++) for (int y = 0; y <= super_h; y++) {
    if (ZQ::point_in_triangle(xs_low + x + 0.5f, ys_low + y + 0.5f, xs0, ys0, xs1, ys1, xs2, ys2)) {
        buffer[4 * (x + y * super_w)    ] = color.r;
        buffer[4 * (x + y * super_w) + 1] = color.g;
        buffer[4 * (x + y * super_w) + 2] = color.b;
        buffer[4 * (x + y * super_w) + 3] = color.a;
    }
  }
//  printf("\tbuffer is filled\n");

  // downsample and blending
  ZQ::downsample(buffer, downsampled, box_w, box_h, sample_rate);
//  printf("\tdownsampled\n");

  float r, g, b, a;
  for (int x = 0; x < box_w; x++) for (int y = 0; y < box_h; y++) {
    r = downsampled[4 * (x + y * box_w)    ];
    g = downsampled[4 * (x + y * box_w) + 1];
    b = downsampled[4 * (x + y * box_w) + 2];
    a = downsampled[4 * (x + y * box_w) + 3];
    ZQ::alpha_blend_pixel(render_target, target_w, target_h, x_low + x, y_low + y, r, g, b, a);
  }

//  printf("\tblended\n");
}

void SoftwareRendererImp::rasterize_image( float x0, float y0,
                                           float x1, float y1,
                                           Texture& tex ) {
  // Task 6: 
  // Implement image rasterization

}

// resolve samples to render target
void SoftwareRendererImp::resolve( void ) {

  // Task 4: 
  // Implement supersampling
  // You may also need to modify other functions marked with "Task 4".
  return;

}


} // namespace CMU462
