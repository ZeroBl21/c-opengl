#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#define GLFW_DLL
#include <GLFW/glfw3.h>
#define GL_LOG_FILE "gl.log"

#include "include/cglm/types-struct.h"
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

void mesh_draw(Mesh* mesh, Shader* shader);
void mesh_setup(Mesh* mesh);
Mesh* mesh_create(Vertex* vertices, GLuint* indices, Texture* textures,
                  GLuint numVertices, GLuint numIndices, GLuint numTextures);
void mesh_destroy(Mesh* mesh);

inline Mesh* mesh_create(Vertex* vertices, GLuint* indices, Texture* textures,
                         GLuint numVertices, GLuint numIndices,
                         GLuint numTextures) {
  Mesh* mesh = (Mesh*)malloc(sizeof(Mesh));
  if (mesh == NULL) {
    // Handle allocation failure
    fprintf(stderr, "Error: failed to allocate mesh\n");
    return NULL;
  }

  mesh->vertices = vertices;
  mesh->indices = indices;
  mesh->textures = textures;
  mesh->numVertices = numVertices;
  mesh->numIndices = numIndices;
  mesh->numTextures = numTextures;

  mesh_setup(mesh);

  return mesh;
}

inline void mesh_destroy(Mesh* mesh) {
  if (mesh != NULL) {
    free(mesh->vertices);
    free(mesh->indices);
    free(mesh->textures);
    free(mesh);
  }
}

inline void mesh_setup(Mesh* mesh) {
  glGenVertexArrays(1, &mesh->VAO);
  glGenBuffers(1, &mesh->VBO);
  glGenBuffers(1, &mesh->EBO);

  glBindVertexArray(mesh->VAO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);

  glBufferData(GL_ARRAY_BUFFER, mesh->numVertices * sizeof(Vertex),
               &mesh->vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->numVertices * sizeof(GLuint),
               &mesh->indices[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex),
                        (void*)offsetof(Vertex, Normal));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex),
                        (void*)offsetof(Vertex, TexCoords));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex),
                        (void*)offsetof(Vertex, TexCoords));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(Vertex),
                        (void*)offsetof(Vertex, Tangent));

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, false, sizeof(Vertex),
                        (void*)offsetof(Vertex, Bitangent));

  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 3, GL_FLOAT, false, sizeof(Vertex),
                        (void*)offsetof(Vertex, m_BoneIDs));

  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, false, sizeof(Vertex),
                        (void*)offsetof(Vertex, m_Weights));

  glBindVertexArray(0);
}

inline void mesh_draw(Mesh* mesh, Shader* shader) {
  const char* diffuseTexture = "texture_diffuse";
  const char* specularTexture = "texture_specular";
  const char* normalTexture = "texture_normal";
  const char* heightTexture = "texture_height";

  GLuint diffuseNr = 1;
  GLuint specularNr = 1;
  GLuint normalNr = 1;
  GLuint heightNr = 1;

  for (GLuint i = 0; i < mesh->numTextures; i++) {
    glActiveTexture(GL_TEXTURE0 + i);

    char* number;
    char* name = mesh->textures[i].type;
    if (strcmp(diffuseTexture, name) == 0) {
      sprintf(number, "%d", diffuseNr++);
    }
    if (strcmp(specularTexture, name) == 0) {
      sprintf(number, "%d", specularNr++);
    }
    if (strcmp(normalTexture, name) == 0) {
      sprintf(number, "%d", normalNr++);
    }
    if (strcmp(heightTexture, name) == 0) {
      sprintf(number, "%d", heightNr++);
    }

    char uniformName[50];
    // TODO: Possible Error
    snprintf(uniformName, sizeof(uniformName), "material.%s%s", name, number);
    shader_set_int(shader, uniformName, i);
    glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
  }

  // draw mesh
  glBindVertexArray(mesh->VAO);
  glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  // Back to default
  glActiveTexture(GL_TEXTURE0);
}

#endif
