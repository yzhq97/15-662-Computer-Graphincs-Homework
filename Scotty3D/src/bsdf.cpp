#include "bsdf.h"

#include <algorithm>
#include <iostream>
#include <utility>

using std::min;
using std::max;
using std::swap;

namespace CMU462 {

void make_coord_space(Matrix3x3& o2w, const Vector3D& n) {
  Vector3D z = Vector3D(n.x, n.y, n.z);
  Vector3D h = z;
  if (fabs(h.x) <= fabs(h.y) && fabs(h.x) <= fabs(h.z))
    h.x = 1.0;
  else if (fabs(h.y) <= fabs(h.x) && fabs(h.y) <= fabs(h.z))
    h.y = 1.0;
  else
    h.z = 1.0;

  z.normalize();
  Vector3D y = cross(h, z);
  y.normalize();
  Vector3D x = cross(z, y);
  x.normalize();

  o2w[0] = x;
  o2w[1] = y;
  o2w[2] = z;
}

// Diffuse BSDF //

Spectrum DiffuseBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return albedo * (1.0 / PI);
}

Spectrum DiffuseBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *wi = sampler.get_sample(pdf);
  return f(wo, *wi);
}

// Mirror BSDF //

Spectrum MirrorBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  Vector3D normal(0, 0, 1);
  return reflectance * max(dot(normal, (wo + wi).unit()), 0.0);
}

Spectrum MirrorBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  //(PathTracer):
  // Implement MirrorBSDF
  reflect(wo, wi);
  *pdf = 1.0f;
  return reflectance * (1.0 / abs_cos_theta(*wi));
}

// Glossy BSDF //

/*
Spectrum GlossyBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlossyBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *pdf = 1.0f;
  return reflect(wo, wi, reflectance);
}
*/

// Refraction BSDF //

Spectrum RefractionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum RefractionBSDF::sample_f(const Vector3D& wo, Vector3D* wi,
                                  float* pdf) {
  // TODO (PathTracer):
  // Implement RefractionBSDF
  return Spectrum();
}

// Glass BSDF //

Spectrum GlassBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlassBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // Compute Fresnel coefficient and either reflect or refract based on it.

  if (refract(wo, wi, ior)) {

    double n_i = ior, n_t = 1.0;
    double cos_wi = cos_theta(*wi);
    if (cos_wi > 0.) swap(n_i, n_t);

    double reflect_prob;
    double sin_t = n_i / n_t * sqrt(max(0., 1. - cos_wi * cos_wi));
    double cos_t = sqrt(1. - sin_t * sin_t);

    if (sin_t > 1.) {
      reflect_prob = 1.;
    } else {
      double abs_cos_wi = abs_cos_theta(*wi);
      double Rs = (n_i * abs_cos_wi - n_t * cos_t) / (n_i * abs_cos_wi + n_t * cos_t);
      double Rp = (n_t * abs_cos_wi - n_i * cos_t) / (n_t * abs_cos_wi + n_i * cos_t);
      reflect_prob = (Rs * Rs + Rp * Rp) / 2.;
    }

    if (1. * rand() / RAND_MAX > reflect_prob) {
      *pdf = 1. - reflect_prob;
      double nint2 = n_i * n_i / (n_t * n_t);
      return *pdf * transmittance * nint2 * (1. / abs_cos_theta(*wi));
    }
  }

  reflect(wo, wi);
  *pdf = 1.;
  return reflectance * (1.0 / abs_cos_theta(*wi));

}

void BSDF::reflect(const Vector3D& wo, Vector3D* wi) {
  // Implement reflection of wo about normal (0,0,1) and store result in wi.
  *wi = Vector3D(0, 0, 2 * wo[2]) - wo;
}

bool BSDF::refract(const Vector3D& wo, Vector3D* wi, float ior) {
  // Use Snell's Law to refract wo surface and store result ray in wi.
  // Return false if refraction does not occur due to total internal reflection
  // and true otherwise. When dot(wo,n) is positive, then wo corresponds to a
  // ray entering the surface through vacuum.

  double n_i = ior, n_t = 1;
  double cos_wo = cos_theta(wo);
  if (cos_wo < 0.0) swap(n_i, n_t);

  double sin_wi = sin_theta(wo) * n_t / n_i;
  if (sin_wi > 1.0) return false;

  Vector3D normal(0, 0, 1);

  double sign = cos_wo < 0.0 ? 1 : -1;
  *wi = sign * normal * sqrt(1.0 - sin_wi * sin_wi) - (wo - normal * cos_wo).unit() * sin_wi;

  return true;

}

// Emission BSDF //

Spectrum EmissionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum EmissionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *wi = sampler.get_sample(pdf);
  return Spectrum();
}

}  // namespace CMU462
