#ifndef MODEL_H
#define MODEL_H

#include <stdbool.h>
#include <string.h>

#include "include/assimp/cimport.h"
#include "include/assimp/postprocess.h"
#include "include/assimp/scene.h"
#include "include/stb/stb_ds.h"
#include "include/stb/stb_image.h"
#include "mesh.h"

typedef struct {
  Mesh* meshes;
  Texture* loadedTextures;

  GLuint numMeshes;
  GLuint numLoadedTextures;

  char* directory;
  bool gammaCorrection;
} Model;

Model model_create(char* path);
void model_draw(Model* model, Shader* shader);

// privates
void model_load(Model* model, char* path);
void model_process_node(Model* model, aiNode* node, const aiScene* scene);
Mesh model_process_mesh(Model* model, aiMesh* mesh, const aiScene* scene);

GLuint texture_from_file(const char* path, const char* directory, bool gamma);
Texture* model_load_material_textures(Model* model, aiMaterial* mat,
                                      aiTextureType type, char* typeName,
                                      const char* directory);
void extract_directory(const char* path, char* directory);

inline void model_load(Model* model, char* path) {
  const aiScene* scene =
      aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    fprintf(stderr, "ERROR::ASSIMP::%s\n", aiGetErrorString());
    return;
  }

  char directory[256];
  extract_directory(path, directory);
  model_process_node(model, scene->mRootNode, scene);

  aiReleaseImport(scene);
}

inline void model_draw(Model* model, Shader* shader) {
  for (GLuint i = 0; i < model->numMeshes; i++) {
    mesh_draw(&model->meshes[i], shader);
  }
}

inline void model_process_node(Model* model, aiNode* node,
                               const aiScene* scene) {
  // process all the node's meshes (if any)
  for (GLuint i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    // meshes->push_back(processMesh(mesh, scene));
    arrpush(model->meshes, model_process_mesh(model, mesh, scene));
    model->numMeshes++;
  }
  // then do the same for each of its children
  for (GLuint i = 0; i < node->mNumChildren; i++) {
    model_process_node(model, node->mChildren[i], scene);
  }
}

inline Mesh model_process_mesh(Model* model, aiMesh* mesh,
                               const aiScene* scene) {
  Vertex* vertices;
  GLuint* indices;
  Texture* textures;

  GLuint numVertices;
  GLuint numIndices;
  // GLuint numTextures;

  for (GLuint i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    vertex.Position = (vec3s){
        .x = mesh->mVertices[i].x,
        .y = mesh->mVertices[i].y,
        .z = mesh->mVertices[i].z,
    };

    // if (mesh->HasNormals())
    if (mesh->mNormals != NULL && mesh->mNumVertices > 0) {
      vertex.Normal = (vec3s){
          .x = mesh->mNormals[i].x,
          .y = mesh->mNormals[i].y,
          .z = mesh->mNormals[i].z,
      };
    }

    if (mesh->mTextureCoords[0]) {
      vertex.TexCoords = (vec2s){
          .x = mesh->mTextureCoords[0][i].x,
          .y = mesh->mTextureCoords[0][i].y,
      };

      vertex.Tangent = (vec3s){
          .x = mesh->mTangents[i].x,
          .y = mesh->mTangents[i].y,
          .z = mesh->mTangents[i].z,
      };

      vertex.Bitangent = (vec3s){
          .x = mesh->mBitangents[i].x,
          .y = mesh->mBitangents[i].y,
          .z = mesh->mBitangents[i].z,
      };
    } else {
      vertex.TexCoords = (vec2s){0};
    }

    arrpush(vertices, vertex);
    numVertices++;
  }

  // Indices
  for (GLuint i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (GLuint j = 0; j < face.mNumIndices; j++) {
      arrpush(indices, face.mIndices[j]);
      numIndices++;
    }
  }

  aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

  // 1. diffuse maps
  Texture* diffuseMaps =
      model_load_material_textures(model, material, aiTextureType_DIFFUSE,
                                   (char*)"texture_diffuse", model->directory);
  GLuint numDiffuseMaps =
      aiGetMaterialTextureCount(material, aiTextureType_DIFFUSE);
  // 2. Specular maps
  Texture* specularMaps =
      model_load_material_textures(model, material, aiTextureType_SPECULAR,
                                   (char*)"texture_specular", model->directory);
  GLuint numSpecularMaps =
      aiGetMaterialTextureCount(material, aiTextureType_SPECULAR);

  // 3. Normal maps
  Texture* normalMaps =
      model_load_material_textures(model, material, aiTextureType_HEIGHT,
                                   (char*)"texture_normal", model->directory);
  GLuint numNormalMaps =
      aiGetMaterialTextureCount(material, aiTextureType_HEIGHT);

  // 4. Height maps
  Texture* heightMaps =
      model_load_material_textures(model, material, aiTextureType_AMBIENT,
                                   (char*)"texture_height", model->directory);
  GLuint numHeightMaps =
      aiGetMaterialTextureCount(material, aiTextureType_AMBIENT);

  // Combine all textures into a single array
  GLuint numTextures =
      numDiffuseMaps + numSpecularMaps + numNormalMaps + numHeightMaps;
  textures = (Texture*)malloc(numTextures * sizeof(Texture));

  GLuint index = 0;
  for (GLuint i = 0; i < numDiffuseMaps; ++i) {
    textures[index++] = diffuseMaps[i];
  }
  for (GLuint i = 0; i < numSpecularMaps; ++i) {
    textures[index++] = specularMaps[i];
  }
  for (GLuint i = 0; i < numNormalMaps; ++i) {
    textures[index++] = normalMaps[i];
  }
  for (GLuint i = 0; i < numHeightMaps; ++i) {
    textures[index++] = heightMaps[i];
  }

  // Free individual texture arrays
  free(diffuseMaps);
  free(specularMaps);
  free(normalMaps);
  free(heightMaps);

  return (Mesh){
      .vertices = vertices,
      .indices = indices,
      .textures = textures,

      .numVertices = numVertices,
      .numIndices = numIndices,
      .numTextures = numTextures,
  };
}

inline void extract_directory(const char* path, char* directory) {
  // Find the last occurrence of '/' in 'path'
  const char* lastSlash = strrchr(path, '/');

  // If '/' is found, extract the directory part
  if (lastSlash != NULL) {
    size_t length = lastSlash - path;
    strncpy(directory, path, length);
    directory[length] = '\0';  // Null-terminate the directory string
  } else {
    // Handle the case when '/' is not found in 'path'
    strcpy(directory, path);
  }
}

inline Texture* model_load_material_textures(Model* model, aiMaterial* mat,
                                             aiTextureType type, char* typeName,
                                             const char* directory) {
  Texture* textures = NULL;

  for (GLuint i = 0; i < aiGetMaterialTextureCount(mat, type); i++) {
    struct aiString str;
    if (aiGetMaterialTexture(mat, type, i, &str) != AI_SUCCESS) {
      continue;
    }

    bool skip = false;
    for (GLuint j = 0; j < arrlen(model->numLoadedTextures); j++) {
      if (strcmp(model->loadedTextures[j].path, str.data) == 0) {
        arrpush(textures, model->loadedTextures[j]);
        skip = true;  // a texture with the same filepath has already been
                      // loaded, continue to next one. (optimization)
        break;
      }
    }

    if (!skip) {
      // if texture hasn't been loaded already, load it
      Texture texture;
      texture.id = texture_from_file(str.data, (char*)directory, false);
      texture.type = typeName;
      texture.path = str.data;
      arrpush(textures, texture);
      // store it as texture loaded for the entire model, to ensure we won't
      // unnecessarily load duplicate textures.
      arrpush(model->loadedTextures, texture);
    }
  }

  return textures;
}

inline GLuint texture_from_file(const char* path, const char* directory,
                                bool gamma) {
  char* filename = (char*)malloc(strlen(path) + strlen(directory) + 2);
  sprintf(filename, "%s/%s", directory, path);

  GLuint textureID;
  glGenTextures(1, &textureID);

  stbi_set_flip_vertically_on_load(true);
  // load and generate the texture
  int width, height, nrChannels;
  GLubyte* data = stbi_load(filename, &width, &height, &nrChannels, 0);
  if (!data) {
    fprintf(stderr, "ERROR: Failed to load texture at path: %s\n", path);
    stbi_image_free(data);
    return 0;
  }

  GLenum format;
  if (nrChannels == 1) format = GL_RED;
  if (nrChannels == 3) format = GL_RGB;
  if (nrChannels == 4) format = GL_RGBA;

  // set the texture wrapping/filtering options (on the currently bound texture
  // object)
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);
  free(filename);

  return textureID;
}

#endif
