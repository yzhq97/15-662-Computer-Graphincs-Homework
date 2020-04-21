#include "environment_light.h"

using std::min;
using std::max;
using std::sin;
using std::cos;
using std::acos;
using std::rand;
using std::vector;

namespace CMU462 {
namespace StaticScene {

EnvironmentLight::EnvironmentLight(const HDRImageBuffer* envMap)
    : envMap(envMap) {
  // (PathTracer) initialize things here as needed

  double w = envMap->w;
  double h = envMap->h;
  const vector<Spectrum> & envmap = envMap->data;

  std::vector<double> p_theta;
  std::vector<std::vector<double>> p_posterior;

  p_theta.resize(h, 0);
  p_joint.resize(h, vector<double>(w));
  p_posterior.resize(h, vector<double>(w));

  double sum = 0.;

  for (int i = 0; i < h; i++) for (int j = 0; j < w; j++) {
      p_joint[i][j] = sin(PI * (i + 0.5) / h) * envmap[i * w + j].illum();
    sum += p_joint[i][j];
  }

  for (int i = 0; i < h; i++) for (int j = 0; j < w; j++) p_joint[i][j] = p_joint[i][j] / sum;
  for (int i = 0; i < h; i++) for (int j = 0; j < w; j++) p_theta[i] += p_joint[i][j];
  for (int i = 0; i < h; i++) for (int j = 0; j < w; j++) p_posterior[i][j] = p_joint[i][j] / (p_theta[i] + EPS_D);

  // accumulate
  p_theta_acc.resize(h, 0);
  p_theta_acc[0] = p_theta[0];
  for (int i = 1; i < h;i++) p_theta_acc[i] = p_theta_acc[i - 1] + p_theta[i];

  p_posterior_acc.resize(h, vector<double>(w));
  for (int i = 0; i < h; i++) {
    p_posterior_acc[i][0] = p_posterior[i][0];
    for (int j = 1; j < w; j++) p_posterior_acc[i][j] = p_posterior_acc[i][j-1] + p_posterior[i][j];
  }
}

void EnvironmentLight::importance_sample(Vector3D *wi, float *pdf) const {

  double w = envMap->w;
  double h = envMap->h;
  double theta, phi, x, y, piv, prev;
  int i, j;
  vector<double>::const_iterator it;

  piv = 1. * rand() / RAND_MAX * p_theta_acc.back();
  it = lower_bound(p_theta_acc.begin(), p_theta_acc.end(), piv); // binary search
  i = it - p_theta_acc.begin();
  prev = i > 0 ? *(it - 1) : 0;
  y = i + (piv - prev) / (*it - prev);
  theta = min(y / h, 1.0) * PI;

  piv = 1. * rand() / RAND_MAX * p_posterior_acc[i].back();
  it = lower_bound(p_posterior_acc[i].begin(), p_posterior_acc[i].end(), piv);
  j = it - p_posterior_acc[i].begin();
  prev = j > 0 ? *(it - 1) : 0;
  x = j + (piv - prev) / (*it - prev);
  phi = min(x / w, 1.0) * 2 * PI;

  *pdf = p_joint[i][j] / (sin(theta) * (2 * PI / w) * (PI / h));
  *wi = Vector3D(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
}

Spectrum EnvironmentLight::sample_L(const Vector3D& p, Vector3D* wi,
                                    float* distToLight, float* pdf) const {
  importance_sample(wi, pdf);
  *distToLight = INF_D;
  Ray r(p, *wi);
  return sample_dir(r);
}

Spectrum EnvironmentLight::interpolate(double x, double y) const {

  double w = envMap->w;
  double h = envMap->h;
  const vector<Spectrum> & envmap = envMap->data;

  long right = lround(x), left, v = lround(y);
  double u1 = right - x + .5, v1;
  if (right == 0 || right == w) {
    left = w - 1;
    right = 0;
  } else left = right - 1;
  if (v == 0) v1 = v = 1; else if (v == h) {
    v = h - 1;
    v1 = 0;
  } else v1 = v - y + .5;
  auto bottom = w * v, top = bottom - w;
  auto u0 = 1 - u1;

  return (envmap[top + left] * u1 + envmap[top + right] * u0) * v1 +
         (envmap[bottom + left] * u1 + envmap[bottom + right] * u0) * (1 - v1);
}

Spectrum EnvironmentLight::sample_dir(const Ray& r) const {

  // (PathTracer) Implement

  double w = envMap->w;
  double h = envMap->h;

  double theta = acos(r.d.y);
  double phi = atan2(-r.d.z, r.d.x) + PI;
  double x = phi / 2. / PI * w;
  double y = theta / PI * h;

  return interpolate(x, y);
}

}  // namespace StaticScene
}  // namespace CMU462
