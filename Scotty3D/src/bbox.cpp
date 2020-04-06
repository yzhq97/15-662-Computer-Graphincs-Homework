#include "bbox.h"

#include "GL/glew.h"

#include <algorithm>
#include <iostream>

namespace CMU462 {

bool BBox::intersect(const Ray &r, double &t0, double &t1) const {
  // Implement ray - bounding box intersection test
  // If the ray intersected the bounding box within the range given by
  // t0, t1, update t0 and t1 with the new intersection times.

  double t_min = r.min_t;
  double t_max = r.max_t;

  Vector3D v[2];
  v[0] = min;
  v[1] = max;

  double new_t_min = (v[r.sign[0]].x - r.o.x) * r.inv_d.x;
  double new_t_max = (v[r.sign[0] ^ 1].x - r.o.x) * r.inv_d.x;

  if (new_t_min > t_min) t_min = new_t_min;
  if (new_t_max < t_max) t_max = new_t_max;
  if (t_min > t_max) return false;

  new_t_min = (v[r.sign[1]].y - r.o.y) * r.inv_d.y;
  new_t_max = (v[r.sign[1] ^ 1].y - r.o.y) * r.inv_d.y;
  if (new_t_min > t_min) t_min = new_t_min;
  if (new_t_max < t_max) t_max = new_t_max;
  if (t_min > t_max) return false;

  new_t_min = (v[r.sign[2]].z - r.o.z) * r.inv_d.z;
  new_t_max = (v[r.sign[2] ^ 1].z - r.o.z) * r.inv_d.z;
  if (new_t_min > t_min) t_min = new_t_min;
  if (new_t_max < t_max) t_max = new_t_max;
  if (t_min > t_max) return false;

  t0 = t_min;
  t1 = t_max;

  return true;

}

void BBox::draw(Color c) const {
  glColor4f(c.r, c.g, c.b, c.a);

  // top
  glBegin(GL_LINE_STRIP);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(max.x, max.y, max.z);
  glEnd();

  // bottom
  glBegin(GL_LINE_STRIP);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, min.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glEnd();

  // side
  glBegin(GL_LINES);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(min.x, min.y, max.z);
  glEnd();
}

std::ostream &operator<<(std::ostream &os, const BBox &b) {
  return os << "BBOX(" << b.min << ", " << b.max << ")";
}

}  // namespace CMU462
