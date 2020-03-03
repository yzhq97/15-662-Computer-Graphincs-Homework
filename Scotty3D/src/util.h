#ifndef CMU462_UTIL_H
#define CMU462_UTIL_H

#include <iostream>
#include "halfEdgeMesh.h"

using namespace std;

namespace util {
  HalfedgeIter find_previous(HalfedgeIter h) {
    int degree = h->face()->degree();
    HalfedgeIter p = h;
    for (int i = 0; i < degree - 1; i++) p = p->next();
    //    while (p->next() != h) p = p->next();
    return p;
  }
}

#endif //CMU462_UTIL_H
