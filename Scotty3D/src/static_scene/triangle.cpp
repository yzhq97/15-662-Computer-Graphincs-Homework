#include "triangle.h"

#include "CMU462/CMU462.h"
#include "GL/glew.h"

namespace CMU462 {
namespace StaticScene {

Triangle::Triangle(const Mesh* mesh, vector<size_t>& v) : mesh(mesh), v(v) {}
Triangle::Triangle(const Mesh* mesh, size_t v1, size_t v2, size_t v3)
    : mesh(mesh), v1(v1), v2(v2), v3(v3) {}

BBox Triangle::get_bbox() const {
  // compute the bounding box of the triangle

  Vector3D p0 = mesh->positions[v1];
  Vector3D p1 = mesh->positions[v2];
  Vector3D p2 = mesh->positions[v3];

  Vector3D min, max;

  min.x = std::min(std::min(p0.x, p1.x), p2.x);
  min.y = std::min(std::min(p0.y, p1.y), p2.y);
  min.z = std::min(std::min(p0.z, p1.z), p2.z);

  max.x = std::max(std::max(p0.x, p1.x), p2.x);
  max.y = std::max(std::max(p0.y, p1.y), p2.y);
  max.z = std::max(std::max(p0.z, p1.z), p2.z);

  return BBox(min, max);

}

bool Triangle::intersect(const Ray& r) const {

  Vector3D p0 = mesh->positions[v1];
  Vector3D p1 = mesh->positions[v2];
  Vector3D p2 = mesh->positions[v3];

  Vector3D e1 = p1 - p0;
  Vector3D e2 = p2 - p0;
  Vector3D s = r.o - p0;

  double det = dot(cross(e1, r.d), e2);

  double u = dot(-cross(s, e2), r.d) / det;
  if (u <= 0.0 || u >= 1.0) return false;

  double v = dot(cross(e1, r.d), s) / det;
  if (v <= 0.0 || u+v >= 1.0) return false;

  double t = dot(-cross(s, e2), e1) / det;

  if (t >= r.min_t || t <= r.max_t) {
    r.max_t = t;
    return true;
  }

  return false;

}

bool Triangle::intersect(const Ray& r, Intersection* isect) const {
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly

  Vector3D p0 = mesh->positions[v1];
  Vector3D p1 = mesh->positions[v2];
  Vector3D p2 = mesh->positions[v3];

  Vector3D e1 = p1 - p0;
  Vector3D e2 = p2 - p0;
  Vector3D s = r.o - p0;

  double det = dot(cross(e1, r.d), e2);

  double u = dot(-cross(s, e2), r.d) / det;
  if (u <= 0.0 || u >= 1.0) return false;

  double v = dot(cross(e1, r.d), s) / det;
  if (v <= 0.0 || u+v >= 1.0) return false;

  double t = dot(-cross(s, e2), e1) / det;

  if (t >= r.min_t || t <= r.max_t) {

    r.max_t = t;
    isect->t = t;
    isect->primitive = this;
    isect->bsdf = get_bsdf();

    Vector3D normal = cross(e1, e2);
    normal.normalize();
    if (dot(normal, r.d) > 0.0) normal = -normal;
    isect->n = normal;

    return true;
  }

  return false;

}

void Triangle::draw(const Color& c) const {
  glColor4f(c.r, c.g, c.b, c.a);
  glBegin(GL_TRIANGLES);
  glVertex3d(mesh->positions[v1].x, mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x, mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x, mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}

void Triangle::drawOutline(const Color& c) const {
  glColor4f(c.r, c.g, c.b, c.a);
  glBegin(GL_LINE_LOOP);
  glVertex3d(mesh->positions[v1].x, mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x, mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x, mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}

}  // namespace StaticScene
}  // namespace CMU462
