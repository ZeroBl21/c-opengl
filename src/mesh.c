#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mesh.h"

Mesh* mesh_create(Vertex* vertices, GLuint* indices, Texture* textures,
                  GLuint numVertices, GLuint numIndices, GLuint numTextures) {
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

void mesh_destroy(Mesh* mesh) {
  if (mesh != NULL) {
    free(mesh->vertices);
    free(mesh->indices);
    free(mesh->textures);
    free(mesh);
  }
}

// Configuración de los buffers y arrays para el renderizado del mesh
void mesh_setup(Mesh* mesh) {
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
                        (void*)offsetof(Vertex, Position));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex),
                        (void*)offsetof(Vertex, Normal));

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

// Renderizado del mesh con el shader especificado
void mesh_draw(Mesh* mesh, Shader* shader) {
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

    char number[4];  // espacio para los números de texturas
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

  // Draw the mesh
  glBindVertexArray(mesh->VAO);
  glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  // Back to default
  glActiveTexture(GL_TEXTURE0);
}
