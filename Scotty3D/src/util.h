#ifndef CMU462_UTIL_H
#define CMU462_UTIL_H

#include <iostream>
#include "halfEdgeMesh.h"

using namespace std;

namespace util {
  HalfedgeIter find_previous(HalfedgeIter h) {
    HalfedgeIter p = h->next();
    while (p->next() != h) p = p->next();
    return p;
  }
}

#endif //CMU462_UTIL_H
