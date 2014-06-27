#include "datatype.h"
#include "helper.h"
#include <math.h>

namespace ccsubdiv {



static void calc_facepoints(mesh_ptr input_mesh, mesh_ptr result_mesh) {
  for (auto& face: input_mesh->faces) {
    result_mesh->vertices.push_back(SubdivHelper::centerpoint(face)); 
    face->facepoint = result_mesh->vertices.back();
  }
}


static void calc_edgepoints(mesh_ptr input_mesh, mesh_ptr result_mesh) {
  for (auto& edge: input_mesh->edges) {
    vertex_ptr vert = std::make_shared<Vertex>();
    auto next_edge = edge->next;
    assert(next_edge);
    vert->coord = next_edge->vert->coord + edge->vert->coord;
    vert->coord += edge->face->facepoint->coord;
    vert->coord += edge->pair->face->facepoint->coord;
    vert->coord /= 4;
    result_mesh->vertices.push_back(vert);
    edge->edgepoint = edge->pair->edgepoint
                    = result_mesh->vertices.back();
  }
}

static void calc_new_vertices(mesh_ptr input_mesh, mesh_ptr result_mesh) {
  for (auto vert: input_mesh->vertices) {
    vertex_ptr new_vert(new Vertex);
    SubdivHelper::average_of_adjacent_facepoints(vert, new_vert);
    size_t sz = SubdivHelper::average_of_adjacent_edgepoints(vert, new_vert);
    new_vert->coord += vert->coord * (sz - 3);
    new_vert->coord /= sz;
    result_mesh->vertices.push_back(new_vert);
    vert->newpoint = result_mesh->vertices.back();
  }
}

static void calc_vertices(mesh_ptr input_mesh, mesh_ptr result_mesh) {
  calc_facepoints(input_mesh, result_mesh);
  calc_edgepoints(input_mesh, result_mesh);
  calc_new_vertices(input_mesh, result_mesh);
}


static void connect_edges(mesh_ptr input_mesh,
                          mesh_ptr result_mesh) {
  for (auto& face : input_mesh->faces) {
    hedge_ptr pe = face->edge;
    do {
      SubdivHelper::split_face_by_edge(face, pe, result_mesh);
      pe = pe->next;
    } while (pe != face->edge);
  }
}

static void connect_edges2(mesh_ptr input_mesh,
                           mesh_ptr result_mesh) {
  for (auto& face : input_mesh->faces) {
    hedge_ptr pe = face->edge;
    do {
      std::vector<vertex_ptr> vertices;
      vertices.push_back(face->facepoint);
      vertices.push_back(pe->edgepoint);
      pe = pe->next;
      vertices.push_back(pe->vert->newpoint);
      vertices.push_back(pe->edgepoint);
      SubdivHelper::create_face(vertices, result_mesh);
    } while (pe != face->edge);
  }
}

static void ccsubdivision_core(mesh_ptr input_mesh,
                               mesh_ptr result_mesh) {
  calc_vertices(input_mesh, result_mesh); 
  connect_edges2(input_mesh, result_mesh);
}


void ccsubdivision(mesh_ptr input_mesh, const size_t n,
                   mesh_ptr result_mesh) {
  std::vector< mesh_ptr > meshes;
  meshes.push_back( input_mesh );
  for (size_t i = 0; i < n; ++i) {
    mesh_ptr sp = std::make_shared<Mesh>();
    ccsubdivision_core(meshes.back(), sp);
    meshes.push_back(sp); 
  }
  result_mesh = meshes.back();
}



} // namespace ccsubdiv



