#ifndef CCSUBDIV_READ_OBJ_FILE_H_
#define CCSUBDIV_READ_OBJ_FILE_H_

#include <fstream>

#include "datatype.h"

namespace ccsubdiv {

class Reader {
public:
  explicit Reader(const char* filename) : ifs(filename), mesh(new Mesh) {}
  ~Reader() { ifs.close(); }
  mesh_handle load_obj_file();

private:
  Reader(const Reader&);
  Reader& operator=(const Reader&);

  void create_face(std::vector<size_t>&);
  void read_vertex(const std::string&);
  //void read_texture_coord(const std::string&);
  void read_normal(const std::string&);
  //void read_param_space_vertex(const std::string&);
  void read_face(const std::string&);

  enum LineType {
    COMMENT,
    VERTEX,
    TEXTURE_COORD,
    NORMAL,
    PARAM_SPACE_VERTEX,
    FACE,
    UNKNOWN
  };
  static LineType get_line_type(const std::string&);

  std::ifstream ifs;
  mesh_handle mesh;
  std::vector<vec3d> normals;
};

}



#endif
