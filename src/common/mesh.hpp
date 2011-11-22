#pragma once

#include "shader.hpp"

enum MeshTypes {HUNTER, DESTROYER, UFO, ASTEROID, MESH_COUNT};

class Mesh
{
public:
  ~Mesh();

  void draw(const Shader& s);

  static Mesh* get_mesh(MeshTypes type);
  static void release_mesh(Mesh* m);

private:
  // a reference counter for shapes
  unsigned int ref_count;

  union {
  	struct {
  		GLuint ibo, vbo;
  	};
  	GLuint bufobjs[2];
  };
  size_t len;

  Mesh(MeshTypes type);
};

