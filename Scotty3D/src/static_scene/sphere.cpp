#include "sphere.h"

#include <cmath>

#include "../bsdf.h"
#include "../misc/sphere_drawing.h"

namespace CMU462 {
namespace StaticScene {

bool Sphere::test(const Ray& r, double& t1, double& t2) const {
  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.

  Vector3D v = r.o - o;

  double a = 2.0 * dot(r.d, r.d);
  double b = 2.0 * dot(r.d, v);
  double c = dot(v, v) - this->r * this->r;
  double d = b * b - 2.0f * a * c;

  if (d <= 0.0) return false;

  d = sqrt(d);
  double t = -(b + d) / a;

  if (t < r.min_t) t = -(b - d) / a;
  if (t < r.min_t || t > r.max_t) return false;

  t1 = -(b + d) / a;
  t2 = -(b - d) / a;

  return true;

}

bool Sphere::intersect(const Ray& r) const {
  // Implement ray - sphere intersection.
  // Note that you might want to use the the Sphere::test helper here.

  double t0, t1;

  if (test(r, t0, t1)) {
    r.max_t = (t0 > r.min_t) ? t0 : t1;
    return true;
  }

  return false;
}

bool Sphere::intersect(const Ray& r, Intersection* isect) const {
  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.

  double t0, t1;

  if (test(r, t0, t1)) {

    r.max_t = (t0 > r.min_t) ? t0 : t1;

    isect->t = r.max_t;
    isect->primitive = this;
    isect->bsdf = get_bsdf();

    Vector3D hit = r.o + isect->t * r.d;
    Vector3D normal = hit - o;
    normal.normalize();
    isect->n = normal;

    return true;
  }

  return false;
}

void Sphere::draw(const Color& c) const { Misc::draw_sphere_opengl(o, r, c); }

void Sphere::drawOutline(const Color& c) const {
  // Misc::draw_sphere_opengl(o, r, c);
}

}  // namespace StaticScene
}  // namespace CMU462
