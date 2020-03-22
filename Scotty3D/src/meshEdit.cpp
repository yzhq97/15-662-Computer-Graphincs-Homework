#include <stdio.h>
#include <float.h>
#include <assert.h>
#include "meshEdit.h"
#include "mutablePriorityQueue.h"
#include "error_dialog.h"
#include "util.h"

namespace CMU462 {

set<int> HalfedgeMesh::getVertexNeighbors(VertexIter v) {
  assignSubdivisionIndices();
  set<int> neighbors;
  HalfedgeIter h = v->halfedge();
  do {
    neighbors.insert(h->twin()->vertex()->index);
    h = h->twin()->next();
  } while(h != v->halfedge());

  return neighbors;
}

VertexIter HalfedgeMesh::splitEdge(EdgeIter e0) {

  if (e0->halfedge()->face()->degree() != 3 && e0->halfedge()->twin()->face()->degree() != 3) {
    showError("Only works with triangle mesh.", false);
    return e0->halfedge()->vertex();
  }

  // Handle boundary
  if (e0->isBoundary()) {

    // COLLECT

    HalfedgeIter h0 = e0->halfedge();
    HalfedgeIter h1 = h0->next();
    HalfedgeIter h2 = h1->next();
    HalfedgeIter h3 = h0->twin();
    HalfedgeIter h6 = h1->twin();
    HalfedgeIter h7 = h2->twin();

    VertexIter v0 = h0->vertex();
    VertexIter v1 = h1->vertex();
    VertexIter v2 = h2->vertex();

    EdgeIter e1 = h1->edge();
    EdgeIter e2 = h2->edge();

    FaceIter f0 = h0->face();

    // NEW

    HalfedgeIter h10 = newHalfedge();
    HalfedgeIter h11 = newHalfedge();
    HalfedgeIter h14 = newHalfedge();
    HalfedgeIter h15 = newHalfedge();

    VertexIter vn = newVertex();

    EdgeIter e5 = newEdge();
    EdgeIter e7 = newEdge();

    FaceIter f2 = newFace();

    // ASSIGN

    vn->position = e0->centroid();

    h10->setNeighbors(e5, h11, vn, f2, h3->next());
    h11->setNeighbors(e5, h10, v0, f2, h14);
    h14->setNeighbors(e7, h15, vn, f2, h2);
    h15->setNeighbors(e7, h14, v2, f0, h0);

    h0->setNeighbors(h0->edge(), h0->twin(), vn, h0->face(), h0->next());
    h1->setNeighbors(h1->edge(), h1->twin(), h1->vertex(), h1->face(), h15);
    h2->setNeighbors(h2->edge(), h2->twin(), h2->vertex(), f2, h11);
    h3->setNeighbors(h3->edge(), h3->twin(), h3->vertex(), h3->face(), h10);

    vn->halfedge() = h0;
    e5->halfedge() = h11;
    e7->halfedge() = h14;
    f2->halfedge() = h2;

    e0->halfedge() = h0;
    f0->halfedge() = h1;

//    checkConsistency();

    return vn;

  } else {

    // COLLECT

    HalfedgeIter h0 = e0->halfedge();
    HalfedgeIter h1 = h0->next();
    HalfedgeIter h2 = h1->next();
    HalfedgeIter h3 = h0->twin();
    HalfedgeIter h4 = h3->next();
    HalfedgeIter h5 = h4->next();
    HalfedgeIter h6 = h1->twin();
    HalfedgeIter h7 = h2->twin();
    HalfedgeIter h8 = h4->twin();
    HalfedgeIter h9 = h5->twin();

    VertexIter v0 = h0->vertex();
    VertexIter v1 = h3->vertex();
    VertexIter v2 = h2->vertex();
    VertexIter v3 = h5->vertex();

    EdgeIter e1 = h1->edge();
    EdgeIter e2 = h2->edge();
    EdgeIter e3 = h4->edge();
    EdgeIter e4 = h5->edge();

    FaceIter f0 = h0->face();
    FaceIter f1 = h3->face();

    // NEW

    HalfedgeIter h10 = newHalfedge();
    HalfedgeIter h11 = newHalfedge();
    HalfedgeIter h12 = newHalfedge();
    HalfedgeIter h13 = newHalfedge();
    HalfedgeIter h14 = newHalfedge();
    HalfedgeIter h15 = newHalfedge();

    VertexIter vn = newVertex();

    EdgeIter e5 = newEdge();
    EdgeIter e6 = newEdge();
    EdgeIter e7 = newEdge();

    FaceIter f2 = newFace();
    FaceIter f3 = newFace();

    // ASSIGN

    vn->position = e0->centroid();

    h10->setNeighbors(e5, h11, vn, f3, h4);
    h11->setNeighbors(e5, h10, v0, f2, h14);
    h12->setNeighbors(e6, h13, v3, f3, h10);
    h13->setNeighbors(e6, h12, vn, f1, h5);
    h14->setNeighbors(e7, h15, vn, f2, h2);
    h15->setNeighbors(e7, h14, v2, f0, h0);

    h0->setNeighbors(h0->edge(), h0->twin(), vn, h0->face(), h0->next());
    h1->setNeighbors(h1->edge(), h1->twin(), h1->vertex(), h1->face(), h15);
    h2->setNeighbors(h2->edge(), h2->twin(), h2->vertex(), f2, h11);
    h3->setNeighbors(h3->edge(), h3->twin(), h3->vertex(), h3->face(), h13);
    h4->setNeighbors(h4->edge(), h4->twin(), h4->vertex(), f3, h12);

    vn->halfedge() = h0;
    e5->halfedge() = h10;
    e6->halfedge() = h13;
    e7->halfedge() = h14;

    f2->halfedge() = h2;
    f3->halfedge() = h4;

    e0->halfedge() = h0;
    f0->halfedge() = h1;
    f1->halfedge() = h5;

//    checkConsistency();

    return vn;
  }
}



VertexIter HalfedgeMesh::collapseEdge(EdgeIter e) {
  // This method should collapse the given edge and return an iterator to
  // the new vertex created by the collapse.

  // CHECK
  HalfedgeIter h0 = e->halfedge(); HalfedgeIter h1 = h0->twin();
  VertexIter v0 = h0->vertex(); VertexIter v1 = h1->vertex();

  set<int> intersect;
  set<int> v0_neighbors = getVertexNeighbors(v0);
  set<int> v1_neighbors = getVertexNeighbors(v1);
  set_intersection(v0_neighbors.begin(), v0_neighbors.end(), v1_neighbors.begin(), v1_neighbors.end(), inserter(intersect,intersect.begin()));

  int n_inter = intersect.size();
  if (n_inter != 2) {
    showError("Collapse edge will cause non-manifold mesh.", false);
    v0->collapseEdgeSuccess = false;
    return v0;
  }

  // COLLECT
  HalfedgeIter h2 = h0->next(); HalfedgeIter h3 = h1->next();
  HalfedgeIter h4 = h2->twin(); HalfedgeIter h5 = h3->twin();
  HalfedgeIter h0_prev = util::find_previous(h0);
  HalfedgeIter h1_prev = util::find_previous(h1);
  FaceIter f0 = h0->face();
  FaceIter f1 = h1->face();

  // TRIANGLE 1
  if (h2->next() == h0_prev) {

    // COLLECT
    HalfedgeIter h4_next = h4->next();
    HalfedgeIter h4_prev = util::find_previous(h4);

    EdgeIter et0 = h2->edge();
    VertexIter vt0 = h2->vertex();
    VertexIter vt1 = h4->vertex();
    FaceIter ft0 = h2->face();
    FaceIter ft1 = h4->face();

    // ASSIGN
    h0->next() = h4_next;
    h4_prev->next() = h0_prev;
    h0->face() = ft1;
    h0_prev->face() = ft1;

    vt0->halfedge() = h1;
    vt1->halfedge() = h0_prev;
    ft1->halfedge() = h4_next;

    // DELETE
    deleteEdge(et0);
    deleteFace(ft0);
    deleteHalfedge(h4);
    deleteHalfedge(h2);
  }

  // TRIANGLE 2
  if (h3->next() == h1_prev) {

    // COLLECT
    HalfedgeIter h5_next = h5->next();
    HalfedgeIter h5_prev = util::find_previous(h5);

    EdgeIter et1 = h3->edge();
    VertexIter vt0 = h3->vertex();
    VertexIter vt1 = h5->vertex();
    FaceIter ft0 = h3->face();
    FaceIter ft1 = h5->face();

    // ASSIGN
    h1->next() = h5_next;
    h5_prev->next() = h1_prev;
    h1->face() = ft1;
    h1_prev->face() = ft1;

    vt0->halfedge() = h0;
    vt1->halfedge() = h1_prev;
    ft1->halfedge() = h5_next;

    // DELETE
    deleteEdge(et1);
    deleteFace(ft0);
    deleteHalfedge(h5);
    deleteHalfedge(h3);
  }

  // ASSIGN

  // set halfedges around v1
  HalfedgeIter p = h0->next();
  do {
    p->vertex() = v0;
    p = p->twin()->next();
  } while (p != h1);
  h0_prev->next() = h0->next();
  h1_prev->next() = h1->next();

  v0->halfedge() = h0_prev->twin();
  v0->position = e->centroid();

  f0->halfedge() = h0_prev;
  f1->halfedge() = h1_prev;

  // DELETE
  deleteVertex(v1);
  deleteEdge(e);
  deleteHalfedge(h0);
  deleteHalfedge(h1);

//  checkConsistency();
  v0->collapseEdgeSuccess = true;

  return v0;

}

VertexIter HalfedgeMesh::collapseFace(FaceIter f) {
  // TODO: (meshEdit)
  // This method should collapse the given face and return an iterator to
  // the new vertex created by the collapse.
  showError("collapseFace() not implemented.");
  return VertexIter();
}

FaceIter HalfedgeMesh::eraseVertex(VertexIter v) {
  // TODO: (meshEdit)
  // This method should replace the given vertex and all its neighboring
  // edges and faces with a single face, returning the new face.

  return FaceIter();
}

FaceIter HalfedgeMesh::eraseEdge(EdgeIter e) {
  // TODO: (meshEdit)
  // This method should erase the given edge and return an iterator to the
  // merged face.

  showError("eraseVertex() not implemented.");
  return FaceIter();
}

EdgeIter HalfedgeMesh::flipEdge(EdgeIter e0) {
  // This method should flip the given edge and return an iterator to the
  // flipped edge.

  if (e0->isBoundary()) return e0;

  // COLLECT
  HalfedgeIter h0 = e0->halfedge(); HalfedgeIter h1 = h0->twin();
  HalfedgeIter h2 = h0->next(); HalfedgeIter h3 = h2->twin();
  HalfedgeIter h4 = h1->next(); HalfedgeIter h5 = h4->twin();
  HalfedgeIter h6 = util::find_previous(h0); HalfedgeIter h7 = h6->twin();
  HalfedgeIter h8 = util::find_previous(h1); HalfedgeIter h9 = h8->twin();

  VertexIter v0 = h1->vertex();
  VertexIter v1 = h3->vertex();
  VertexIter v2 = h0->vertex();
  VertexIter v3 = h5->vertex();
  VertexIter v4 = h6->vertex();
  VertexIter v5 = h8->vertex();

  EdgeIter e1 = h2->edge();
  EdgeIter e2 = h4->edge();
  EdgeIter e3 = h6->edge();
  EdgeIter e4 = h8->edge();

  FaceIter f0 = h0->face();
  FaceIter f1 = h1->face();

  // ASSIGN
  h0->setNeighbors(e0, h1, v3, f0, h2->next());
  h1->setNeighbors(e0, h0, v1, f1, h4->next());
  h2->setNeighbors(e1, h3, v0, f1, h1);
  h3->setNeighbors(e1, h2, v1, h3->face(), h3->next());
  h4->setNeighbors(e2, h5, v2, f0, h0);
  h5->setNeighbors(e2, h4, v3, h5->face(), h5->next());
  h6->setNeighbors(e3, h7, v4, f0, h4);
  h7->setNeighbors(e3, h6, v2, h7->face(), h7->next());
  h8->setNeighbors(e4, h9, v5, f1, h2);
  h9->setNeighbors(e4, h8, v0, h9->face(), h9->next());

  v0->halfedge() = h2;
  v1->halfedge() = h1;
  v2->halfedge() = h4;
  v3->halfedge() = h0;
  v4->halfedge() = h6;
  v5->halfedge() = h8;

  e0->halfedge() = h0;
  e1->halfedge() = h2;
  e2->halfedge() = h4;
  e3->halfedge() = h6;
  e4->halfedge() = h8;

  f0->halfedge() = h0;
  f1->halfedge() = h1;

//  checkConsistency();

  return e0;
}

void HalfedgeMesh::subdivideQuad(bool useCatmullClark) {
  // Unlike the local mesh operations (like bevel or edge flip), we will perform
  // subdivision by splitting *all* faces into quads "simultaneously."  Rather
  // than operating directly on the halfedge data structure (which as you've
  // seen
  // is quite difficult to maintain!) we are going to do something a bit nicer:
  //
  //    1. Create a raw list of vertex positions and faces (rather than a full-
  //       blown halfedge mesh).
  //
  //    2. Build a new halfedge mesh from these lists, replacing the old one.
  //
  // Sometimes rebuilding a data structure from scratch is simpler (and even
  // more
  // efficient) than incrementally modifying the existing one.  These steps are
  // detailed below.

  // TODO Step I: Compute the vertex positions for the subdivided mesh.  Here
  // we're
  // going to do something a little bit strange: since we will have one vertex
  // in
  // the subdivided mesh for each vertex, edge, and face in the original mesh,
  // we
  // can nicely store the new vertex *positions* as attributes on vertices,
  // edges,
  // and faces of the original mesh.  These positions can then be conveniently
  // copied into the new, subdivided mesh.
  // [See subroutines for actual "TODO"s]
  if (useCatmullClark) {
    computeCatmullClarkPositions();
  } else {
    computeLinearSubdivisionPositions();
  }

  // TODO Step II: Assign a unique index (starting at 0) to each vertex, edge,
  // and
  // face in the original mesh.  These indices will be the indices of the
  // vertices
  // in the new (subdivided mesh).  They do not have to be assigned in any
  // particular
  // order, so long as no index is shared by more than one mesh element, and the
  // total number of indices is equal to V+E+F, i.e., the total number of
  // vertices
  // plus edges plus faces in the original mesh.  Basically we just need a
  // one-to-one
  // mapping between original mesh elements and subdivided mesh vertices.
  // [See subroutine for actual "TODO"s]
  assignSubdivisionIndices();

  // TODO Step III: Build a list of quads in the new (subdivided) mesh, as
  // tuples of
  // the element indices defined above.  In other words, each new quad should be
  // of
  // the form (i,j,k,l), where i,j,k and l are four of the indices stored on our
  // original mesh elements.  Note that it is essential to get the orientation
  // right
  // here: (i,j,k,l) is not the same as (l,k,j,i).  Indices of new faces should
  // circulate in the same direction as old faces (think about the right-hand
  // rule).
  // [See subroutines for actual "TODO"s]
  vector<vector<Index> > subDFaces;
  vector<Vector3D> subDVertices;
  buildSubdivisionFaceList(subDFaces);
  buildSubdivisionVertexList(subDVertices);

  // TODO Step IV: Pass the list of vertices and quads to a routine that clears
  // the
  // internal data for this halfedge mesh, and builds new halfedge data from
  // scratch,
  // using the two lists.
  rebuild(subDFaces, subDVertices);
}

/**
 * Compute new vertex positions for a mesh that splits each polygon
 * into quads (by inserting a vertex at the face midpoint and each
 * of the edge midpoints).  The new vertex positions will be stored
 * in the members Vertex::newPosition, Edge::newPosition, and
 * Face::newPosition.  The values of the positions are based on
 * simple linear interpolation, e.g., the edge midpoints and face
 * centroids.
 */
void HalfedgeMesh::computeLinearSubdivisionPositions() {

  // For each vertex, assign Vertex::newPosition to
  // its original position, Vertex::position.
  for (VertexIter v = verticesBegin(); v != verticesEnd(); v++) {
    v->newPosition = v->position;
  }

  // For each edge, assign the midpoint of the two original
  // positions to Edge::newPosition.
  for (EdgeIter e = edgesBegin(); e != edgesEnd(); e++) {
    e->newPosition = e->centroid();
  }

  // For each face, assign the centroid (i.e., arithmetic mean)
  // of the original vertex positions to Face::newPosition.  Note
  // that in general, NOT all faces will be triangles!
  for (FaceIter f = facesBegin(); f != facesEnd(); f++) {
    f->newPosition = f->centroid();
  }

}

/**
 * Compute new vertex positions for a mesh that splits each polygon
 * into quads (by inserting a vertex at the face midpoint and each
 * of the edge midpoints).  The new vertex positions will be stored
 * in the members Vertex::newPosition, Edge::newPosition, and
 * Face::newPosition.  The values of the positions are based on
 * the Catmull-Clark rules for subdivision.
 */
void HalfedgeMesh::computeCatmullClarkPositions() {
  // The implementation for this routine should be
  // a lot like HalfedgeMesh::computeLinearSubdivisionPositions(),
  // except that the calculation of the positions themsevles is
  // slightly more involved, using the Catmull-Clark subdivision
  // rules. (These rules are outlined in the Developer Manual.)

  Vector3D weight = {1.f/8.f, 3.f/4.f, 1.f/8.f};

  // ASSIGN FACES

  for (FaceIter f = facesBegin(); f != facesEnd(); f++) {
    f->newPosition = f->centroid();
  }

  // ASSIGN EDGES

  for (EdgeIter e = edgesBegin(); e != edgesEnd(); e++) {

    Vector3D end0 = e->halfedge()->vertex()->position;
    Vector3D end1 = e->halfedge()->next()->vertex()->position;

    Vector3D p0 = e->halfedge()->face()->newPosition;
    Vector3D p1 = e->halfedge()->twin()->face()->newPosition;

    if (e->isBoundary()) e->newPosition = (end0 + end1) / 2.0f;
    else e->newPosition = (p0 + p1 + end0 + end1) / 4.0f;
  }

  // ASSIGN VERTICES

  for (VertexIter v = verticesBegin(); v != verticesEnd(); v++) {
    int n = v->degree();

    HalfedgeIter ht0 = v->halfedge();
    Vector3D q = ht0->face()->newPosition; // 1st of the new face positions to be summed
    Vector3D r = ht0->edge()->newPosition; // 1st of the new edge positions to be summed

    if (v->isBoundary()){
      HalfedgeIter ht1 = v->halfedge();
      Vector3D r = {0.,0.,0.};
      int cnt = 0;

      if ( ht1->edge()->isBoundary() ) {
        r = r + ht1->edge()->newPosition;
        cnt++;
      }

      ht1 = ht1->twin()->next();

      while (ht1 != v->halfedge()) {
        if (ht1->edge()->isBoundary()) {
          r = r + ht1->edge()->newPosition;
          cnt++;
        }
        ht1 = ht1->twin()->next();
      }

      printf("assign edge->vertex %d times\n", cnt);

      Vector3D R = r;
      v->newPosition = ( R + 6.0*(v->position)) / 8.0;

    }
    else {

      ht0 = ht0->twin()->next();
      while (ht0 != v->halfedge()) {
        q = q + ht0->face()->newPosition;
        r = r + ht0->edge()->newPosition;
        ht0 = ht0->twin()->next();
      }

      Vector3D Q = q / n;
      Vector3D R = r / n;

      v->newPosition = (Q + 2 * R + (n - 3) * (v->position)) / n;

    }
  }

}

/**
 * Assign a unique integer index to each vertex, edge, and face in
 * the mesh, starting at 0 and incrementing by 1 for each element.
 * These indices will be used as the vertex indices for a mesh
 * subdivided using Catmull-Clark (or linear) subdivision.
 */
void HalfedgeMesh::assignSubdivisionIndices() {

  // Start a counter at zero; if you like, you can use the
  // "Index" type (defined in halfedgeMesh.h)
  Index i = 0;

  // Iterate over vertices, assigning values to Vertex::index
  for (VertexIter v = verticesBegin(); v != verticesEnd(); v++) {
    v->index = i;
    i++;
  }

  // Iterate over edges, assigning values to Edge::index
  for (EdgeIter e = edgesBegin(); e != edgesEnd(); e++) {
    e->index = i;
    i++;
  }

  // Iterate over faces, assigning values to Face::index
  for (FaceIter f = facesBegin(); f != facesEnd(); f++) {
    f->index = i;
    i++;
  }

}

/**
 * Build a flat list containing all the vertex positions for a
 * Catmull-Clark (or linear) subdivison of this mesh.  The order of
 * vertex positions in this list must be identical to the order
 * of indices assigned to Vertex::newPosition, Edge::newPosition,
 * and Face::newPosition.
 */
void HalfedgeMesh::buildSubdivisionVertexList(vector<Vector3D>& subDVertices) {

  // Iterate over vertices, assigning Vertex::newPosition to the
  // appropriate location in the new vertex list.
  for (VertexIter v = verticesBegin(); v != verticesEnd(); v++) {
    subDVertices.push_back(v->newPosition);
  }

  // Iterate over edges, assigning Edge::newPosition to the appropriate
  // location in the new vertex list.
  for (EdgeIter e = edgesBegin(); e != edgesEnd(); e++) {
    subDVertices.push_back(e->newPosition);
  }

  // Iterate over faces, assigning Face::newPosition to the appropriate
  // location in the new vertex list.
  for (FaceIter f = facesBegin(); f != facesEnd(); f++) {
    subDVertices.push_back(f->newPosition);
  }

}

/**
 * Build a flat list containing all the quads in a Catmull-Clark
 * (or linear) subdivision of this mesh.  Each quad is specified
 * by a vector of four indices (i,j,k,l), which come from the
 * members Vertex::index, Edge::index, and Face::index.  Note that
 * the ordering of these indices is important because it determines
 * the orientation of the new quads; it is also important to avoid
 * "bowties."  For instance, (l,k,j,i) has the opposite orientation
 * of (i,j,k,l), and if (i,j,k,l) is a proper quad, then (i,k,j,l)
 * will look like a bowtie.
 */
void HalfedgeMesh::buildSubdivisionFaceList(vector<vector<Index> >& subDFaces) {
  // TODO This routine is perhaps the most tricky step in the construction of
  // a subdivision mesh (second, perhaps, to computing the actual Catmull-Clark
  // vertex positions).  Basically what you want to do is iterate over faces,
  // then for each for each face, append N quads to the list (where N is the
  // degree of the face).  For this routine, it may be more convenient to simply
  // append quads to the end of the list (rather than allocating it ahead of
  // time), though YMMV.  You can of course iterate around a face by starting
  // with its first halfedge and following the "next" pointer until you get
  // back to the beginning.  The tricky part is making sure you grab the right
  // indices in the right order---remember that there are indices on vertices,
  // edges, AND faces of the original mesh.  All of these should get used.  Also
  // remember that you must have FOUR indices per face, since you are making a
  // QUAD mesh!

  // iterate over faces
  for (FaceIter f = facesBegin(); f != facesEnd(); f++) {
    HalfedgeIter h = f->halfedge();
    // loop around face (loop over the halfedges of the face)
    HalfedgeIter h0 = h;
    for(int i =0; i<f->degree(); i++) {
      h = h->next();
      // build lists of four indices for each sub-quad
      vector<Index> quad(4);
      quad[0] = h0->edge()->index;
      quad[1] = h->vertex()->index;
      quad[2] = h->edge()->index;
      quad[3] = f->index;
      // append each list of four indices to face list
      subDFaces.push_back(quad);
      h0 = h;
    }
  }
}

FaceIter HalfedgeMesh::bevelVertex(VertexIter v) {
  // TODO This method should replace the vertex v with a face, corresponding to
  // a bevel operation. It should return the new face.  NOTE: This method is
  // responsible for updating the *connectivity* of the mesh only---it does not
  // need to update the vertex positions.  These positions will be updated in
  // HalfedgeMesh::bevelVertexComputeNewPositions (which you also have to
  // implement!)

  showError("bevelVertex() not implemented.");
  return facesBegin();
}

FaceIter HalfedgeMesh::bevelEdge(EdgeIter e) {
  // TODO This method should replace the edge e with a face, corresponding to a
  // bevel operation. It should return the new face.  NOTE: This method is
  // responsible for updating the *connectivity* of the mesh only---it does not
  // need to update the vertex positions.  These positions will be updated in
  // HalfedgeMesh::bevelEdgeComputeNewPositions (which you also have to
  // implement!)

  showError("bevelEdge() not implemented.");
  return facesBegin();
}

FaceIter HalfedgeMesh::bevelFace(FaceIter f) {
  // (and ring of faces around it), corresponding to a bevel operation. It
  // should return temp new face.  NOTE: This method is responsible for updating
  // temp *connectivity* of temp mesh only---it does not need to update temp vertex
  // positions.  These positions will be updated in
  // HalfedgeMesh::bevelFaceComputeNewPositions (which you also have to
  // implement!)

  if (f->isBoundary()) return f;

  int degree = f->degree();

  HalfedgeIter fhe = f->halfedge();

  HalfedgeIter temp;
  vector<VertexIter> new_v;
  vector<FaceIter> new_f;
  vector<HalfedgeIter> new_he;
  vector<EdgeIter> new_e;

  // NEW
  for (int i = 0; i < degree; i++)
  {
    new_v.push_back(newVertex());
    new_f.push_back(newFace());
    new_he.push_back(newHalfedge());
    new_he.push_back(newHalfedge());
    new_he.push_back(newHalfedge());
    new_he.push_back(newHalfedge());
    new_e.push_back(newEdge());
    new_e.push_back(newEdge());
  }

  // ASSIGN
  for (int i = 0; i < degree; i++)
  {

    // ASSIGN OLD
    fhe->face() = new_f[i];
    fhe->next()->vertex()->halfedge() = new_he[4 * i];

    // ASSIGN NEW
    new_f[i]->halfedge() = new_he[4 * i + 2];
    new_e[2 * i]->halfedge() = new_he[4 * i + 1];
    new_e[2 * i + 1]->halfedge() = new_he[4 * i + 2];
    new_v[i]->halfedge() = new_he[4 * i + 1];

    new_he[4 * i]->setNeighbors(new_e[2 * i], new_he[4 * i + 1], fhe->next()->vertex(), new_f[i], new_he[4 * i + 2]);
    new_he[4 * i + 1]->setNeighbors(new_e[2 * i], new_he[4 * i], new_v[i], new_f[(i + 1) % degree], fhe->next());
    new_he[4 * i + 2]->setNeighbors(new_e[2 * i + 1], new_he[4 * i + 3], new_v[i], new_f[i], new_he[4 * ((i + degree - 1) % degree) + 1]);
    new_he[4 * i + 3]->setNeighbors(new_e[2 * i + 1], new_he[4 * i + 2], new_v[(i + degree - 1) % degree], f, new_he[4 * ((i + 1) % degree) + 3]);

    new_v[i]->position = fhe->next()->vertex()->position;

    temp = fhe;
    fhe = fhe->next();
    temp->next() = new_he[4 * i];
  }

  f->halfedge() = new_he[3];

//  checkConsistency();

  return f;
}


void HalfedgeMesh::bevelFaceComputeNewPositions(
    vector<Vector3D>& originalVertexPositions,
    vector<HalfedgeIter>& newHalfedges, double normalShift,
    double tangentialInset) {
  // TODO Compute new vertex positions for the vertices of the beveled face.
  //
  // These vertices can be accessed via newHalfedges[i]->vertex()->position for
  // i = 1, ..., newHalfedges.size()-1.
  //
  // The basic strategy here is to loop over the list of outgoing halfedges,
  // and use the preceding and next vertex position from the original mesh
  // (in the originalVertexPositions array) to compute an offset vertex
  // position.
  //
  // Note that there is a 1-to-1 correspondence between halfedges in
  // newHalfedges and vertex positions
  // in orig.  So, you can write loops of the form
  //
  // for( int i = 0; i < newHalfedges.size(); hs++ )
  // {
  //    Vector3D pi = originalVertexPositions[i]; // get the original vertex
  //    position correponding to vertex i
  // }
  //

  FaceIter f = newHalfedges[0]->twin()->next()->twin()->face();

  Vector3D f_norm = f->normal();
  Vector3D f_center = f->centroid();

  int n = originalVertexPositions.size();

  for (int i = 0; i < n; i++) {
    Vector3D pi = originalVertexPositions[i];
    newHalfedges[i]->vertex()->position = newHalfedges[i]->vertex()->position + f_norm * normalShift + (f_center - pi) * tangentialInset;
  }

}

void HalfedgeMesh::bevelVertexComputeNewPositions(
    Vector3D originalVertexPosition, vector<HalfedgeIter>& newHalfedges,
    double tangentialInset) {
  // TODO Compute new vertex positions for the vertices of the beveled vertex.
  //
  // These vertices can be accessed via newHalfedges[i]->vertex()->position for
  // i = 1, ..., hs.size()-1.
  //
  // The basic strategy here is to loop over the list of outgoing halfedges,
  // and use the preceding and next vertex position from the original mesh
  // (in the orig array) to compute an offset vertex position.

}

void HalfedgeMesh::bevelEdgeComputeNewPositions(
    vector<Vector3D>& originalVertexPositions,
    vector<HalfedgeIter>& newHalfedges, double tangentialInset) {
  // TODO Compute new vertex positions for the vertices of the beveled edge.
  //
  // These vertices can be accessed via newHalfedges[i]->vertex()->position for
  // i = 1, ..., newHalfedges.size()-1.
  //
  // The basic strategy here is to loop over the list of outgoing halfedges,
  // and use the preceding and next vertex position from the original mesh
  // (in the orig array) to compute an offset vertex position.
  //
  // Note that there is a 1-to-1 correspondence between halfedges in
  // newHalfedges and vertex positions
  // in orig.  So, you can write loops of the form
  //
  // for( int i = 0; i < newHalfedges.size(); i++ )
  // {
  //    Vector3D pi = originalVertexPositions[i]; // get the original vertex
  //    position correponding to vertex i
  // }
  //

}

void HalfedgeMesh::splitPolygons(vector<FaceIter>& fcs) {
  for (auto f : fcs) splitPolygon(f);
}

void HalfedgeMesh::splitPolygon(FaceIter f) {
  // TODO: (meshedit) 
  // Triangulate a polygonal face

  if (f->degree() == 3) { return; }

  int n_edges = f->degree() - 3;
  int n_halves = 2 * n_edges;

  // COLLECT

  VertexIter v = f->halfedge()->vertex();
  HalfedgeIter h0 = f->halfedge();
  HalfedgeIter h1 = h0->next();
  v->halfedge() = h0;

  // NEW

  HalfedgeIter ht0, ht1;

  vector<EdgeIter> edges;
  vector<FaceIter> faces;
  vector<HalfedgeIter> halves;

  for (int i = 0; i < n_edges; i++) {
    edges.push_back(newEdge());
    faces.push_back(newFace());
    halves.push_back(newHalfedge());
    halves.push_back(newHalfedge());
  }

  // ASSIGN

  ht0 = h1->next();
  edges[0]->halfedge() = halves[0];
  h1->next() = halves[0];
  halves[0]->setNeighbors(edges[0], halves[1], ht0->vertex(), f, h0);

  for (int j = 0; j < n_edges - 1; j++) {
    ht1 = ht0;
    ht0 = ht0->next();
    halves[2 * j + 1]->setNeighbors(edges[j], halves[2 * j], v, faces[j], ht1);
    halves[2 * j + 2]->setNeighbors(edges[j + 1], halves[2 * j + 3], ht0->vertex(), faces[j], halves[2 * j + 1]);
    ht1->next() = halves[2 * j + 2];
    faces[j]->halfedge() = ht1;
    edges[j + 1]->halfedge() = halves[2 * j + 2];
  }

  ht0->face() = faces[n_edges - 1];
  ht0->next()->face() = faces[n_edges - 1];
  ht0->next()->next() = halves[n_halves - 1];
  halves[n_halves - 1]->setNeighbors(edges[n_edges - 1], halves[n_halves - 2], v, faces[n_edges - 1], ht0);
  faces[n_edges - 1]->halfedge() = ht0;

//  checkConsistency();
}

EdgeRecord::EdgeRecord(EdgeIter& _edge) : edge(_edge) {
  // TODO: (meshEdit)
  // Compute the combined quadric from the edge endpoints.
  // -> Build the 3x3 linear system whose solution minimizes the quadric error
  //    associated with these two endpoints.
  // -> Use this system to solve for the optimal position, and store it in
  //    EdgeRecord::optimalPoint.
  // -> Also store the cost associated with collapsing this edg in
  //    EdgeRecord::Cost.
}

void MeshResampler::upsample(HalfedgeMesh& mesh)
// This routine should increase the number of triangles in the mesh using Loop
// subdivision.
// Compute new positions for all the vertices in the input mesh, using
// the Loop subdivision rule, and store them in Vertex::newPosition.
{
  // -> At this point, we also want to mark each vertex as being a vertex of the
  //    original mesh.

  float n, u;
  for (VertexIter v = mesh.verticesBegin(); v != mesh.verticesEnd(); v++) {
    v->isNew = false;
    n = (float)(v->degree());
    u = (n == 3) ? 3.0f/16.0f : 3.0f / (8.0f * n);
    v->newPosition = (1.0f - u * n) * v->position + u * n * v->neighborhoodCentroid();
  }

  // -> Next, compute the updated vertex positions associated with edges, and
  //    store it in Edge::newPosition.

  for (EdgeIter e = mesh.edgesBegin(); e != mesh.edgesEnd(); e++) {
    e->isNew = false;
    HalfedgeIter temp1 = e->halfedge();
    HalfedgeIter temp2 = temp1->twin();
    Vector3D C = temp1->next()->next()->vertex()->position;
    Vector3D D = temp2->next()->next()->vertex()->position;
    e->newPosition = 3.0f / 8.0f * 2.0f * e->centroid() + 1.0f / 8.0f * (C+D);
  }

  // -> Next, we're going to split every edge in the mesh, in any order.  For
  //    future reference, we're also going to store some information about which
  //    subdivided edges come from splitting an edge in the original mesh, and
  //    which edges are new, by setting the flat Edge::isNew. Note that in this
  //    loop, we only want to iterate over edges of the original mesh.
  //    Otherwise, we'll end up splitting edges that we just split (and the
  //    loop will never end!)

  int n_edges = mesh.nEdges();
  EdgeIter e = mesh.edgesBegin();
  vector<EdgeIter> edges;
  vector<VertexIter> vertices;

  for (int i = 0; i < n_edges; i++) {

    // get the next edge from an iterator
    EdgeIter next = e;
    next++;

    VertexIter v0;
    if (e->isNew == false) {
      edges.push_back(e);
      v0 = mesh.splitEdge(e);
      vertices.push_back(v0);
      v0->isNew = true;
      v0->halfedge()->twin()->next()->edge()->isNew = true;
      v0->halfedge()->next()->next()->edge()->isNew = true;
    }
    e = next;
  }


  /*Flip any new edge that touches a new vertex and an old vertex.
  Note: Every original edge will now be represented by 2 edges,
  you should not flip these edges, because they
  are always already along the boundary of the 4 way divided triangles.
  In the diagrams below, you should only flip the blue edges
  that connect an old and new vertex,
  but you should not flip any of the black new edges.*/

  // -> Now flip any new edge that connects an old and new vertex.
  for (EdgeIter e = mesh.edgesBegin(); e != mesh.edgesEnd(); e++) {
    if ((e->isNew) && (e->halfedge()->vertex()->isNew ^ e->halfedge()->twin()->vertex()->isNew)) {
      mesh.flipEdge(e);
    }
  }

  // -> Finally, copy the new vertex positions into final Vertex::position.
  for (VertexIter v = mesh.verticesBegin(); v != mesh.verticesEnd(); v++) {
    if (!v->isNew) v->position = v->newPosition;
  }

  for (int j = 0; j < vertices.size(); j++) {
    vertices[j]->position = edges[j]->newPosition;
  }

}

void MeshResampler::downsample(HalfedgeMesh& mesh) {
  // TODO: (meshEdit)
  // Compute initial quadrics for each face by simply writing the plane equation
  // for the face in homogeneous coordinates. These quadrics should be stored
  // in Face::quadric
  showError("downsample() not implemented.");
}

void MeshResampler::resample(HalfedgeMesh& mesh) {
  // TODO: (meshEdit)
  // Compute the mean edge length.
  // Repeat the four main steps for 5 or 6 iterations
  // -> Split edges much longer than the target length (being careful about
  //    how the loop is written!)
  // -> Collapse edges much shorter than the target length.  Here we need to
  //    be EXTRA careful about advancing the loop, because many edges may have
  //    been destroyed by a collapse (which ones?)
  // -> Now flip each edge if it improves vertex degree
  // -> Finally, apply some tangential smoothing to the vertex positions
  showError("resample() not implemented.");
}

}  // namespace CMU462
