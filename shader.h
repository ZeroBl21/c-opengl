#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/cglm/types-struct.h"

typedef struct {
  GLuint ID;
  const char* vertex_code;
  const char* fragment_code;
} Shader;

Shader shader_create(const char* vertexPath, const char* fragmentPath);
void shader_use(Shader* shader);
// Primitives
void shader_set_bool(Shader* shader, const char* name, bool value);
void shader_set_int(Shader* shader, const char* name, int value);
void shader_set_float(Shader* shader, const char* name, float value);
// Vectors
void shader_set_vec2(Shader* shader, const char* name, const vec2s value);
void shader_set_vec2f(Shader* shader, const char* name, float x, float y);
void shader_set_vec3(Shader* shader, const char* name, const vec3s value);
void shader_set_vec3f(Shader* shader, const char* name, float x, float y,
                      float z);
void shader_set_vec4(Shader* shader, const char* name, const vec4s value);
void shader_set_vec4f(Shader* shader, const char* name, float x, float y,
                      float z, float w);
// Matrices
void shader_set_mat2(Shader* shader, const char* name, const mat2s mat);
void shader_set_mat3(Shader* shader, const char* name, const mat3s mat);
void shader_set_mat4(Shader* shader, const char* name, const mat4s mat);

static char* read_file(const char* path, const char* modes);

inline Shader shader_create(const char* vertexPath, const char* fragmentPath) {
  Shader shader = {0};

  // 1. Retrieve the vertex/fragment source code from filePath
  shader.vertex_code = read_file(vertexPath, "r");
  shader.fragment_code = read_file(fragmentPath, "r");

  // return shader;
  GLuint vertex, fragment;
  int success;
  char infoLog[512];

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &shader.vertex_code, NULL);
  glCompileShader(vertex);

  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    fprintf(stderr,
            "ERROR: Vertex shader compilation failed\nFile: %s\nDetails:\n%s\n",
            vertexPath, infoLog);
  }

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &shader.fragment_code, NULL);
  glCompileShader(fragment);

  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    fprintf(stderr,
            "ERROR: Fragment shader compilation failed\nFile: %s\nDetails:\n%s\n",
            fragmentPath, infoLog);
  }

  shader.ID = glCreateProgram();
  glAttachShader(shader.ID, vertex);
  glAttachShader(shader.ID, fragment);
  glLinkProgram(shader.ID);

  glGetShaderiv(shader.ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader.ID, 512, NULL, infoLog);
    fprintf(stderr, "ERROR: Shader program linking failed %s\n", infoLog);
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  return shader;
}

inline void shader_use(Shader* shader) { glUseProgram(shader->ID); }

inline void shader_set_bool(Shader* shader, const char* name, bool value) {
  glUniform1i(glGetUniformLocation(shader->ID, name), (int)value);
}

inline void shader_set_int(Shader* shader, const char* name, int value) {
  glUniform1i(glGetUniformLocation(shader->ID, name), value);
}

inline void shader_set_float(Shader* shader, const char* name, float value) {
  glUniform1f(glGetUniformLocation(shader->ID, name), value);
}
// ------------------------------------------------------------------------
inline void shader_set_vec2(Shader* shader, const char* name,
                            const vec2s value) {
  glUniform2fv(glGetUniformLocation(shader->ID, name), 1, value.raw);
}

inline void shader_set_vec2f(Shader* shader, const char* name, float x,
                             float y) {
  glUniform2f(glGetUniformLocation(shader->ID, name), x, y);
}
// ------------------------------------------------------------------------
inline void shader_set_vec3(Shader* shader, const char* name,
                            const vec3s value) {
  glUniform3fv(glGetUniformLocation(shader->ID, name), 1, value.raw);
}
inline void shader_set_vec3f(Shader* shader, const char* name, float x, float y,
                             float z) {
  glUniform3f(glGetUniformLocation(shader->ID, name), x, y, z);
}
// ------------------------------------------------------------------------
inline void shader_set_vec4(Shader* shader, const char* name,
                            const vec4s value) {
  glUniform4fv(glGetUniformLocation(shader->ID, name), 1, value.raw);
}
inline void shader_set_vec4f(Shader* shader, const char* name, float x, float y,
                             float z, float w) {
  glUniform4f(glGetUniformLocation(shader->ID, name), x, y, z, w);
}
// ------------------------------------------------------------------------
inline void shader_set_mat2(Shader* shader, const char* name, const mat2s mat) {
  glUniformMatrix2fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE,
                     *mat.raw);
}
// ------------------------------------------------------------------------
inline void shader_set_mat3(Shader* shader, const char* name, const mat3s mat) {
  glUniformMatrix3fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE,
                     *mat.raw);
}
// ------------------------------------------------------------------------
inline void shader_set_mat4(Shader* shader, const char* name, const mat4s mat) {
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE,
                     *mat.raw);
}

// Privates
static char* read_file(const char* path, const char* modes) {
  FILE* file;

  file = fopen(path, modes);
  if (file == NULL) {
    printf("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n");
    exit(EXIT_FAILURE);
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* text = (char*)malloc(size + 1);

  fread(text, 1, size, file);
  fclose(file);

  text[size] = '\0';

  return text;
}

#endif
