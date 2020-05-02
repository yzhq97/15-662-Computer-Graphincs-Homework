#include "sampler.h"

namespace CMU462 {

// Uniform Sampler2D Implementation //

Vector2D UniformGridSampler2D::get_sample() const {
  // Implement uniform 2D grid sampler

  double x = (double)(rand()) / RAND_MAX;
  double y = (double)(rand()) / RAND_MAX;
  
  return Vector2D(x, y);
}

// Uniform Hemisphere Sampler3D Implementation //

Vector3D UniformHemisphereSampler3D::get_sample() const {
  double Xi1 = (double)(rand()) / RAND_MAX;
  double Xi2 = (double)(rand()) / RAND_MAX;

  double theta = acos(Xi1);
  double phi = 2.0 * PI * Xi2;

  double xs = sinf(theta) * cosf(phi);
  double ys = sinf(theta) * sinf(phi);
  double zs = cosf(theta);

  return Vector3D(xs, ys, zs);
}

Vector3D CosineWeightedHemisphereSampler3D::get_sample() const {
  float f;
  return get_sample(&f);
}

Vector3D CosineWeightedHemisphereSampler3D::get_sample(float *pdf) const {

  double Xi1 = (double)(rand()) / RAND_MAX;
  double Xi2 = (double)(rand()) / RAND_MAX;

  double theta = acos(Xi1);
  double phi = 2.0 * PI * Xi2;

  double xs = sinf(theta) * cosf(phi);
  double ys = sinf(theta) * sinf(phi);
  double zs = cosf(theta);

  double cos_theta = fabs(zs);
  *pdf = cos_theta * M_1_PI;

  return Vector3D(xs, ys, zs);
}

}  // namespace CMU462
