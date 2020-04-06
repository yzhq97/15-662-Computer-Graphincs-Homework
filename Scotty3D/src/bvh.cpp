#include "bvh.h"

#include "CMU462/CMU462.h"
#include "static_scene/triangle.h"
#include "bvhUtil.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CMU462 {
namespace StaticScene {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {
  this->primitives = _primitives;

  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.

  root = BVH_UTIL::build_tree(primitives, 0, primitives.size(), 0, max_leaf_size);
}

BVHAccel::~BVHAccel() {
  // Implement a proper destructor for your BVH accelerator aggregate
  if (root) BVH_UTIL::delete_tree(root);
}

BBox BVHAccel::get_bbox() const { return root->bb; }

bool BVHAccel::intersect(const Ray &ray) const {
  // Implement ray - bvh aggregate intersection test. A ray intersects
  // with a BVH aggregate if and only if it intersects a primitive in
  // the BVH that is not an aggregate.

  if (root) return BVH_UTIL::intersect(primitives, root, ray);
  else return false;

}

bool BVHAccel::intersect(const Ray &ray, Intersection *isect) const {
  // Implement ray - bvh aggregate intersection test. A ray intersects
  // with a BVH aggregate if and only if it intersects a primitive in
  // the BVH that is not an aggregate. When an intersection does happen.
  // You should store the non-aggregate primitive in the intersection data
  // and not the BVH aggregate itself.

  if (root) return BVH_UTIL::intersect(primitives, root, ray, isect);
  else return false;

}

}  // namespace StaticScene
}  // namespace CMU462
