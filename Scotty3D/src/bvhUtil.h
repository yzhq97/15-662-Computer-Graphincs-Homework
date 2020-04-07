#ifndef SCOTTY3D_BVHUTIL_H
#define SCOTTY3D_BVHUTIL_H

#include <iostream>
#include "bvh.h"

using namespace std;

namespace CMU462 {
namespace BVH_UTIL {

  size_t quick_sect(vector<StaticScene::Primitive *> &primitives, double piv_val, size_t axis, size_t start, size_t end) {

    size_t p = start;

    BBox bb;
    double centroid;

    for (int i = start; i < end; i++) {

      bb = primitives[i]->get_bbox();

      if (axis == 0) centroid = bb.centroid().x;
      else if (axis == 1) centroid = bb.centroid().y;
      else centroid = bb.centroid().z;

      if (centroid > piv_val) {
        swap(primitives[p], primitives[i]);
        p += 1;
      }

    }

    return p;
  }


  StaticScene::BVHNode * build_tree(std::vector<StaticScene::Primitive *> &primitives, size_t start, size_t end, size_t axis, size_t max_leaf_size) {

//    printf("build tree called with start=%d, end=%d\n", start, end);

    BBox bb;
    size_t size = end - start;
    for (size_t i = start; i < end; i++) bb.expand(primitives[i]->get_bbox());
    auto * p = new StaticScene::BVHNode(bb, start, size);

    double piv_val;

    if (axis == 0) piv_val = bb.centroid().x;
    else if (axis == 1) piv_val = bb.centroid().y;
    else piv_val = bb.centroid().z;

    if (size > max_leaf_size) {
      size_t pivot = quick_sect(primitives, piv_val, axis, start, end);
      p->l = build_tree(primitives, start, pivot, (axis + 1) % 3, max_leaf_size);
      p->r = build_tree(primitives, pivot, end, (axis + 1) % 3, max_leaf_size);
    }

    return p;

  }

  void delete_tree(StaticScene::BVHNode *p) {
    if (p->l) delete_tree(p->l);
    if (p->r) delete_tree(p->r);
    delete p;
  }

  bool intersect(const vector<StaticScene::Primitive *> & primitives, StaticScene::BVHNode* p, const Ray & ray) {

    double t_min, t_max;
    bool bb_intersect = p->bb.intersect(ray, t_min, t_max);

    if (!bb_intersect) return false;

    if (p->isLeaf()) {

      // check all primitives for leaf

      bool hit = false;
      for (size_t i = p->start; i < p->start + p->range; i++)
        if (primitives[i]->intersect(ray)) hit = true;
      return hit;

    } else {

      // front to back traversal

      bool l, r;

      bool hl, hr;
      double tl0=-1e9, tl1=-1e9, tr0=-1e9, tr1=-1e9;

      hl = p->l && p->l->bb.intersect(ray, tl0, tl1);
      hr = p->r && p->r->bb.intersect(ray, tr0, tr1);

      if (hl && hr) {

        double tl = (tl0 > ray.min_t) ? tl0 : tl1;
        double tr = (tr0 > ray.min_t) ? tr0 : tr1;
        if (tl < tr) {
          l = (p->l && intersect(primitives, p->l, ray));
          r = (p->r && intersect(primitives, p->r, ray));
        } else {
          r = (p->r && intersect(primitives, p->r, ray));
          l = (p->l && intersect(primitives, p->l, ray));
        }

      } else {

        l = p->l && (p->l && intersect(primitives, p->l, ray));
        r = p->r && (p->r && intersect(primitives, p->r, ray));

      }

      return l || r;
    }

  }


  bool intersect(const vector<StaticScene::Primitive *> & primitives, StaticScene::BVHNode* p, const Ray & ray, StaticScene::Intersection* isect) {

    double t0, t1;
    bool bb_intersect = p->bb.intersect(ray, t0, t1);

    if (!bb_intersect) return false;

    if (p->isLeaf()) {

      // check all primitives for leaf

      bool hit = false;
      for (size_t i = p->start; i < p->start + p->range; i++)
        if (primitives[i]->intersect(ray, isect)) hit = true;
      return hit;

    } else {

      // front to back traversal

      bool l, r;

      bool hl, hr;
      double tl0=-1e9, tl1=-1e9, tr0=-1e9, tr1=-1e9;

      hl = p->l && p->l->bb.intersect(ray, tl0, tl1);
      hr = p->r && p->r->bb.intersect(ray, tr0, tr1);

      if (hl && hr) {

        double tl = (tl0 > ray.min_t) ? tl0 : tl1;
        double tr = (tr0 > ray.min_t) ? tr0 : tr1;
        if (tl < tr) {
          l = (p->l && intersect(primitives, p->l, ray, isect));
          r = (p->r && intersect(primitives, p->r, ray, isect));
        } else {
          r = (p->r && intersect(primitives, p->r, ray, isect));
          l = (p->l && intersect(primitives, p->l, ray, isect));
        }

      } else {

        l = p->l && (p->l && intersect(primitives, p->l, ray, isect));
        r = p->r && (p->r && intersect(primitives, p->r, ray, isect));

      }

      return l || r;
    }

  }

}
}

#endif //SCOTTY3D_BVHUTIL_H
