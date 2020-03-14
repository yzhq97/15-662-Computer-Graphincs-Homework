#include <stdio.h>
#include <float.h>
#include <assert.h>
#include "meshEdit.h"
#include "mutablePriorityQueue.h"
#include "error_dialog.h"
#include "util.h"

namespace CMU462 {

std::set<int> HalfedgeMesh::getVertexRing(VertexCIter Vertices) {
  // Iterate over vertices in the 1-ring of v,
  // inserting vertex->index
  // to the vertex ring set, OneRing
  assignSubdivisionIndices();
  std::set<int> OneRing;

  HalfedgeCIter h = Vertices->halfedge();     // get one of the outgoing halfedges of the vertex
  do {
    HalfedgeCIter h_twin = h->twin();// get the vertex of the current halfedge
    VertexCIter v = h_twin->vertex();// vertex is 'source' of the half edge.
    // so h->vertex() is v,
    // whereas h_twin->vertex() is the neighbor vertex.
    //cout << " index= " << v->index << " pos = " << v->position << endl;      // print the vertex position
    OneRing.insert(v->index);               // insert the vertex to the one ring set.

    h = h_twin->next();              // move to the next outgoing halfedge of the vertex.
  } while(h != Vertices->halfedge());         // keep going until we're back at the beginning


  return OneRing;
}

VertexIter HalfedgeMesh::splitEdge(EdgeIter e0) {
  // This method should split the given edge and return an iterator to the
  // newly inserted vertex. The halfedge of this vertex should point along
  // the edge that was split, rather than the new edges.

  if (e0->isBoundary()) return e0->halfedge()->vertex();

  // COLLECT

  HalfedgeIter h0 = e0->halfedge(); HalfedgeIter h1 = h0->twin();

  FaceIter f0 = h0->face(); FaceIter f1 = h1->face();

  if (f0->degree() != 3 || f1->degree() != 3) {
    showError("Edge split only works on triangle meshes.", false);
    return e0->halfedge()->vertex();
  }

  HalfedgeIter h2 = h0->next(); HalfedgeIter h3 = h2->twin();
  HalfedgeIter h4 = h2->next(); HalfedgeIter h5 = h4->twin();
  HalfedgeIter h6 = h1->next(); HalfedgeIter h7 = h6->twin();
  HalfedgeIter h8 = h6->next(); HalfedgeIter h9 = h8->twin();

  EdgeIter e1 = h2->edge();
  EdgeIter e2 = h4->edge();
  EdgeIter e3 = h6->edge();
  EdgeIter e4 = h8->edge();

  VertexIter v0 = h1->vertex();
  VertexIter v1 = h0->vertex();
  VertexIter v2 = h4->vertex();
  VertexIter v3 = h8->vertex();

  // CREATE
  VertexIter v5 = newVertex();

  EdgeIter e5 = newEdge();
  EdgeIter e6 = newEdge();
  EdgeIter e7 = newEdge();
  EdgeIter e8 = newEdge();

  HalfedgeIter h10 = newHalfedge(); HalfedgeIter h11 = newHalfedge();
  HalfedgeIter h12 = newHalfedge(); HalfedgeIter h13 = newHalfedge();
  HalfedgeIter h14 = newHalfedge(); HalfedgeIter h15 = newHalfedge();
  HalfedgeIter h16 = newHalfedge(); HalfedgeIter h17 = newHalfedge();

  FaceIter f2 = newFace();
  FaceIter f3 = newFace();
  FaceIter f4 = newFace();
  FaceIter f5 = newFace();

  // ASSIGN NEW
  v5->position = (v0->position + v1->position) / 2.0f;

  h10->setNeighbors(e5, h11, v5, f2, h2);
  h11->setNeighbors(e5, h10, v0, f5, h17);
  h12->setNeighbors(e6, h13, v2, f2, h10);
  h13->setNeighbors(e6, h12, v5, f3, h4);
  h14->setNeighbors(e7, h15, v1, f3, h13);
  h15->setNeighbors(e7, h14, v5, f4, h6);
  h16->setNeighbors(e8, h17, v3, f4, h15);
  h17->setNeighbors(e8, h16, v5, f5, h8);

  e5->halfedge() = h10;
  e6->halfedge() = h12;
  e7->halfedge() = h14;
  e8->halfedge() = h16;

  v5->halfedge() = h10;

  f2->halfedge() = h10;
  f3->halfedge() = h14;
  f4->halfedge() = h15;
  f5->halfedge() = h11;

  // ASSIGN OLD
  h2->setNeighbors(e1, h3, v0, f2, h12);
  h3->setNeighbors(e1, h2, v2, h3->face(), h3->next());
  h4->setNeighbors(e2, h5, v2, f3, h14);
  h5->setNeighbors(e2, h4, v1, h5->face(), h5->next());
  h6->setNeighbors(e3, h7, v1, f4, h16);
  h7->setNeighbors(e3, h6, v3, h7->face(), h7->next());
  h8->setNeighbors(e4, h9, v3, f5, h11);
  h9->setNeighbors(e4, h8, v0, h9->face(), h9->next());

  e1->halfedge() = h2;
  e2->halfedge() = h4;
  e3->halfedge() = h6;
  e4->halfedge() = h8;

  v0->halfedge() = h2;
  v1->halfedge() = h6;
  v2->halfedge() = h4;
  v3->halfedge() = h8;

  f2->halfedge() = h2;
  f3->halfedge() = h4;
  f4->halfedge() = h6;
  f5->halfedge() = h8;

  // DELETE
  deleteEdge(e0);
  deleteFace(f0);
  deleteFace(f1);
  deleteHalfedge(h0);
  deleteHalfedge(h1);

//  checkConsistency();

  return v5;
}

VertexIter HalfedgeMesh::collapseEdge(EdgeIter e) {
  // This method should collapse the given edge and return an iterator to
  // the new vertex created by the collapse.

  auto vt1 = e->halfedge()->vertex();
  auto vt2 = e->halfedge()->twin()->vertex();

  // get intersection of the one ring of vt1 and vt2
  int num_one_ring_shared_adjacent_verts = 0;
  std::set<int> ring_vt1 = HalfedgeMesh::getVertexRing(vt1);
  std::set<int> ring_vt2 = HalfedgeMesh::getVertexRing(vt2);

  //int len1 = ring_vt1.size();
  //int len2 = ring_vt2.size();
  //std::sort (ring_vt1.begin(), ring_vt1.end() ); // already sorted

  set<int> intersect;
  set_intersection(ring_vt1.begin(),ring_vt1.end(),
                   ring_vt2.begin(),ring_vt2.end(),
                   std::inserter(intersect,intersect.begin()));

  num_one_ring_shared_adjacent_verts = intersect.size();
  cout << "One Ring Intersection = " << num_one_ring_shared_adjacent_verts << endl;

  // for (auto item = intersect.begin(); item != intersect.end(); item++ ) {
  //    cout << *item << endl;
  // }

  if (num_one_ring_shared_adjacent_verts != 2) {
    cout << "Non-manifold Edge Collapse Requested" << endl;
    e->halfedge()->vertex()->collapseSuccess = false;
    return e->halfedge()->vertex();
  }


  auto h = e->halfedge();
  auto h_twin = h->twin();
  auto newVtx = h->vertex();
  auto delVtx = h_twin->vertex();

  auto h1 = e->halfedge()->next();
  auto h2 = e->halfedge()->twin()->next();
  auto h1_twin = h1->twin();
  auto h2_twin = h2->twin();
  auto h1_prev = h1;
  do {
    h1_prev = h1_prev->next();
  } while (h1_prev->next() != h);
  auto h2_prev = h2;
  do {
    h2_prev = h2_prev->next();
  } while (h2_prev->next() != h_twin);

  // Triangular cases
  if (h1->next() == h1_prev) {
    // printf("First!\n");
    auto e1 = h1->edge();
    auto f1 = h1->face();
    auto v1 = h1->vertex();
    auto v2 = h1_twin->vertex();
    auto f_nei = h1_twin->face();
    auto h1_nei_next = h1_twin->next();
    auto h1_nei_prev = h1_nei_next;
    do {
      h1_nei_prev = h1_nei_prev->next();
    } while (h1_nei_prev->next() != h1_twin);

    // half edges
    h->next() = h1_nei_next;
    h1_nei_prev->next() = h1_prev;

    // vertices
    v1->halfedge() = h_twin;
    v2->halfedge() = h1_prev;

    // face
    f_nei->halfedge() = h1_nei_next;
    h->face() = f_nei;
    h1_prev->face() = f_nei;

    // delete
    deleteEdge(e1);
    deleteFace(f1);
    deleteHalfedge(h1_twin);
    deleteHalfedge(h1);
  }

  if (h2->next() == h2_prev) {
    // printf("Second\n");
    auto e2 = h2->edge();
    auto f2 = h2->face();
    auto v1 = h2->vertex();
    auto v2 = h2_twin->vertex();
    auto f_nei = h2_twin->face();
    auto h2_nei_next = h2_twin->next();
    auto h2_nei_prev = h2_nei_next;
    do {
      h2_nei_prev = h2_nei_prev->next();
    } while (h2_nei_prev->next() != h2_twin);

    // half edges
    h_twin->next() = h2_nei_next;
    h2_nei_prev->next() = h2_prev;

    // vertices
    v1->halfedge() = h;
    v2->halfedge() = h2_prev;

    // face
    f_nei->halfedge() = h2_nei_next;
    h_twin->face() = f_nei;
    h2_prev->face() = f_nei;

    // delete
    deleteEdge(e2);
    deleteFace(f2);
    deleteHalfedge(h2_twin);
    deleteHalfedge(h2);
  }

  printf("Get Centroid\n");
  Vector3D centroid = e->centroid();
  auto f1 = h->face();
  auto f2 = h_twin->face();

  // half edges
  auto currH = h->next();
  int n = 0;
  do {
    currH->vertex() = newVtx;
    currH = currH->twin()->next();
    printf("No. %d\n",++n);
  } while (currH != h_twin);
  h1_prev->next() = h->next();
  h2_prev->next() = h_twin->next();
  cout << "past do while edge collapse" << endl;
  // vertex
  newVtx->halfedge() = h1_prev->twin();
  newVtx->position = centroid;

  // face
  f1->halfedge() = h1_prev;
  f2->halfedge() = h2_prev;

  printf("End of Edge Collapse: Cleanup\n");

  deleteVertex(delVtx);
  deleteEdge(e);
  deleteHalfedge(h);
  deleteHalfedge(h_twin);

  printf("New vertex Position\n");
  cout<<newVtx->position<<endl;
  newVtx->collapseSuccess = true;
  return newVtx;

//  if (e->isBoundary()) return e->halfedge()->vertex();
//
//  // COLLECT
//  HalfedgeIter hp = e->halfedge(); HalfedgeIter hq = hp->twin();
//  VertexIter vp = hp->vertex(); VertexIter vq = hq->vertex();
//  FaceIter fp = hp->face(); FaceIter fq = hq->face();
//  int p_degree = fp->degree(); int q_degree = fq->degree();
//
//  if (p_degree == 3 && q_degree == 3) {
//    showError("cannot collapse this edge", false);
//    return e->halfedge()->vertex();
//  }
//
//  HalfedgeIter hp00, hp01, hp10, hp11;
//  HalfedgeIter hq00, hq01, hq10, hq11;
//  HalfedgeIter hp_prev, hq_prev;
//  EdgeIter ep0, ep1, eq0, eq1;
//  VertexIter vpp, vqq;
//
//  if (p_degree == 3) {
//    hp00 = hp->next(); hp01 = hp00->twin();
//    hp10 = hp00->next(); hp11 = hp10->twin();
//    ep0 = hp00->edge(); ep1 = hp10->edge();
//    vpp = hp10->vertex();
//  } else {
//    hp_prev = util::find_previous(hp);
//  }
//
//  if (q_degree == 3) {
//    hq00 = hq->next(); hq01 = hq00->twin();
//    hq10 = hq00->next(); hq11 = hq10->twin();
//    eq0 = hp00->edge(); eq1 = hp10->edge();
//    vqq = hq10->vertex();
//  } else {
//    hq_prev = util::find_previous(hq);
//  }
//
//  // NEW
//  VertexIter v = newVertex();
//  EdgeIter ep, eq;
//  if (p_degree == 3) ep = newEdge();
//  if (q_degree == 3) eq = newEdge();
//
//  // ASSIGN
//  v->position = (vp->position + vq->position) / 2.0f;
//
//  if (p_degree == 3) {
//    hp11->setNeighbors(ep, hp01, v, hp11->face(), hp11->next());
//    hp01->setNeighbors(ep, hp11, vpp, hp01->face(), hp01->next());
//    ep->halfedge() = hp01;
//    v->halfedge() = hp11;
//  } else {
//    hp_prev->next() = hp->next();
//  }
//
//  if (q_degree == 3) {
//    hq11->setNeighbors(eq, hq01, v, hq11->face(), hq11->next());
//    hq01->setNeighbors(eq, hq11, vqq, hq01->face(), hq01->next());
//    eq->halfedge() = hq01;
//    v->halfedge() = hq11;
//  } else {
//    hq_prev->next() = hq->next();
//  }
//
//  if (p_degree > 3 && q_degree > 3) v->halfedge() = vp->halfedge()->twin()->next();
//
//  HalfedgeIter p = vp->halfedge();
//  do {
//    p->vertex() = v;
//    p = p->twin()->next();
//  } while (p != vp->halfedge());
//
//  HalfedgeIter q = vq->halfedge();
//  do {
//    q->vertex() = v;
//    q = q->twin()->next();
//  } while (q != vq->halfedge());
//
//  // DELETE
//
//  deleteEdge(e);
//  deleteVertex(vp);
//  deleteVertex(vq);
//  deleteHalfedge(hp);
//  deleteHalfedge(hq);
//
//  if (p_degree == 3) {
//    deleteEdge(ep0); deleteEdge(ep1);
//    deleteHalfedge(hp00); deleteHalfedge(hp10);
//    deleteFace(fp);
//  }
//
//  if (q_degree == 3) {
//    deleteEdge(eq0); deleteEdge(eq1);
//    deleteHalfedge(hq00); deleteHalfedge(hq10);
//    deleteFace(fq);
//  }
//
////  checkConsistency();
//
//  return v;
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
  // TODO For each vertex, assign Vertex::newPosition to
  // its original position, Vertex::position.

  // TODO For each edge, assign the midpoint of the two original
  // positions to Edge::newPosition.

  // TODO For each face, assign the centroid (i.e., arithmetic mean)
  // of the original vertex positions to Face::newPosition.  Note
  // that in general, NOT all faces will be triangles!
  showError("computeLinearSubdivisionPositions() not implemented.");
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
  // TODO The implementation for this routine should be
  // a lot like HalfedgeMesh::computeLinearSubdivisionPositions(),
  // except that the calculation of the positions themsevles is
  // slightly more involved, using the Catmull-Clark subdivision
  // rules. (These rules are outlined in the Developer Manual.)

  // TODO face

  // TODO edges

  // TODO vertices
  showError("computeCatmullClarkPositions() not implemented.");
}

/**
 * Assign a unique integer index to each vertex, edge, and face in
 * the mesh, starting at 0 and incrementing by 1 for each element.
 * These indices will be used as the vertex indices for a mesh
 * subdivided using Catmull-Clark (or linear) subdivision.
 */
void HalfedgeMesh::assignSubdivisionIndices() {
  // TODO Start a counter at zero; if you like, you can use the
  // "Index" type (defined in halfedgeMesh.h)

  // TODO Iterate over vertices, assigning values to Vertex::index

  // TODO Iterate over edges, assigning values to Edge::index

  // TODO Iterate over faces, assigning values to Face::index
  showError("assignSubdivisionIndices() not implemented.");
}

/**
 * Build a flat list containing all the vertex positions for a
 * Catmull-Clark (or linear) subdivison of this mesh.  The order of
 * vertex positions in this list must be identical to the order
 * of indices assigned to Vertex::newPosition, Edge::newPosition,
 * and Face::newPosition.
 */
void HalfedgeMesh::buildSubdivisionVertexList(vector<Vector3D>& subDVertices) {
  // TODO Resize the vertex list so that it can hold all the vertices.

  // TODO Iterate over vertices, assigning Vertex::newPosition to the
  // appropriate location in the new vertex list.

  // TODO Iterate over edges, assigning Edge::newPosition to the appropriate
  // location in the new vertex list.

  // TODO Iterate over faces, assigning Face::newPosition to the appropriate
  // location in the new vertex list.
  showError("buildSubdivisionVertexList() not implemented.");
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

  // TODO iterate over faces
  // TODO loop around face
  // TODO build lists of four indices for each sub-quad
  // TODO append each list of four indices to face list
  showError("buildSubdivisionFaceList() not implemented.");
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
  showError("splitPolygon() not implemented.");
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
{
  // TODO: (meshEdit)
  // Compute new positions for all the vertices in the input mesh, using
  // the Loop subdivision rule, and store them in Vertex::newPosition.
  // -> At this point, we also want to mark each vertex as being a vertex of the
  //    original mesh.
  // -> Next, compute the updated vertex positions associated with edges, and
  //    store it in Edge::newPosition.
  // -> Next, we're going to split every edge in the mesh, in any order.  For
  //    future reference, we're also going to store some information about which
  //    subdivided edges come from splitting an edge in the original mesh, and
  //    which edges are new, by setting the flat Edge::isNew. Note that in this
  //    loop, we only want to iterate over edges of the original mesh.
  //    Otherwise, we'll end up splitting edges that we just split (and the
  //    loop will never end!)
  // -> Now flip any new edge that connects an old and new vertex.
  // -> Finally, copy the new vertex positions into final Vertex::position.

  // Each vertex and edge of the original surface can be associated with a
  // vertex in the new (subdivided) surface.
  // Therefore, our strategy for computing the subdivided vertex locations is to
  // *first* compute the new positions
  // using the connectity of the original (coarse) mesh; navigating this mesh
  // will be much easier than navigating
  // the new subdivided (fine) mesh, which has more elements to traverse.  We
  // will then assign vertex positions in
  // the new mesh based on the values we computed for the original mesh.

  // Compute updated positions for all the vertices in the original mesh, using
  // the Loop subdivision rule.

  // Next, compute the updated vertex positions associated with edges.

  // Next, we're going to split every edge in the mesh, in any order.  For
  // future
  // reference, we're also going to store some information about which
  // subdivided
  // edges come from splitting an edge in the original mesh, and which edges are
  // new.
  // In this loop, we only want to iterate over edges of the original
  // mesh---otherwise,
  // we'll end up splitting edges that we just split (and the loop will never
  // end!)

  // Finally, flip any new edge that connects an old and new vertex.

  // Copy the updated vertex positions to the subdivided mesh.
  showError("upsample() not implemented.");
}

void MeshResampler::downsample(HalfedgeMesh& mesh) {
  // TODO: (meshEdit)
  // Compute initial quadrics for each face by simply writing the plane equation
  // for the face in homogeneous coordinates. These quadrics should be stored
  // in Face::quadric
  // -> Compute an initial quadric for each vertex as the sum of the quadrics
  //    associated with the incident faces, storing it in Vertex::quadric
  // -> Build a priority queue of edges according to their quadric error cost,
  //    i.e., by building an EdgeRecord for each edge and sticking it in the
  //    queue.
  // -> Until we reach the target edge budget, collapse the best edge. Remember
  //    to remove from the queue any edge that touches the collapsing edge
  //    BEFORE it gets collapsed, and add back into the queue any edge touching
  //    the collapsed vertex AFTER it's been collapsed. Also remember to assign
  //    a quadric to the collapsed vertex, and to pop the collapsed edge off the
  //    top of the queue.
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
