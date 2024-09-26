#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <stddef.h>
#include <stdbool.h>
#include <GLFW/glfw3.h> // Incluye GLFW para manejar el contexto de OpenGL

#include "cglm/types-struct.h"
#include "shader.h"

#define MAX_BONE_INFLUENCE 4

typedef struct {
  vec3s Position;
  vec3s Normal;
  vec2s TexCoords;
  vec3s Tangent;
  vec3s Bitangent;
  
  // bone indexes which will influence this vertex
  int m_BoneIDs[MAX_BONE_INFLUENCE];
  // weights from each bone
  float m_Weights[MAX_BONE_INFLUENCE];
} Vertex;

typedef struct {
  GLuint id;
  char* type;
  char* path;
} Texture;

typedef struct {
  Vertex* vertices;
  GLuint* indices;
  Texture* textures;
  
  GLuint numVertices;
  GLuint numIndices;
  GLuint numTextures;

  // Render Data
  GLuint VAO, VBO, EBO;
} Mesh;

Mesh* mesh_create(Vertex* vertices, GLuint* indices, Texture* textures,
                  GLuint numVertices, GLuint numIndices, GLuint numTextures);
void mesh_destroy(Mesh* mesh);
void mesh_draw(Mesh* mesh, Shader* shader);
void mesh_setup(Mesh* mesh);

#endif // MESH_H
