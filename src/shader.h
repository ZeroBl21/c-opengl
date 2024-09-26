#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <stdbool.h>

#include "cglm/types-struct.h"

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
void shader_set_vec3f(Shader* shader, const char* name, float x, float y, float z);
void shader_set_vec4(Shader* shader, const char* name, const vec4s value);
void shader_set_vec4f(Shader* shader, const char* name, float x, float y, float z, float w);

// Matrices
void shader_set_mat2(Shader* shader, const char* name, const mat2s mat);
void shader_set_mat3(Shader* shader, const char* name, const mat3s mat);
void shader_set_mat4(Shader* shader, const char* name, const mat4s mat);

#endif // SHADER_H
