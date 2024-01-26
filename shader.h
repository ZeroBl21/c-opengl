#ifndef SHADER_H

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  GLuint ID;
  const char* vertex_code;
  const char* fragment_code;
} Shader;

Shader shader_create(const char* vertexPath, const char* fragmentPath);
void shader_use(Shader* shader);
void shader_set_bool(Shader* shader, const char* name, bool value);
void shader_set_int(Shader* shader, const char* name, int value);
void shader_set_float(Shader* shader, const char* name, float value);

inline Shader shader_create(const char* vertexPath, const char* fragmentPath) {
  Shader shader = {0};

  // 1. Retrieve the vertex/fragment source code from filePath
  FILE* vShaderFile;
  FILE* fShaderFile;

  // Open files
  vShaderFile = fopen(vertexPath, "r");
  fShaderFile = fopen(fragmentPath, "r");

  if (vShaderFile == NULL || fShaderFile == NULL) {
    printf("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n");
    exit(EXIT_FAILURE);
  }

  // Read file's buffer contents into strings
  fseek(vShaderFile, 0, SEEK_END);
  long vSize = ftell(vShaderFile);
  fseek(vShaderFile, 0, SEEK_SET);

  fseek(fShaderFile, 0, SEEK_END);
  long fSize = ftell(fShaderFile);
  fseek(fShaderFile, 0, SEEK_SET);

  char* vShaderCode = (char*)malloc(vSize + 1);
  char* fShaderCode = (char*)malloc(fSize + 1);

  fread(vShaderCode, 1, vSize, vShaderFile);
  fread(fShaderCode, 1, fSize, fShaderFile);

  // Close file handlers
  fclose(vShaderFile);
  fclose(fShaderFile);

  // Null-terminate the strings
  vShaderCode[vSize] = '\0';
  fShaderCode[fSize] = '\0';

  shader.vertex_code = vShaderCode;
  shader.fragment_code = fShaderCode;

  // return shader;
  GLuint vertex, fragment;
  int success;
  char infoLog[512];

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);

  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    fprintf(stderr, "ERROR: vertex shader compilation failed %s\n", infoLog);
  }

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);

  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    fprintf(stderr, "ERROR: fragment shader compilation failed %s\n", infoLog);
  }

  shader.ID = glCreateProgram();
  glAttachShader(shader.ID, vertex);
  glAttachShader(shader.ID, fragment);
  glLinkProgram(shader.ID);

  glGetShaderiv(shader.ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader.ID, 512, NULL, infoLog);
    fprintf(stderr, "ERROR: shader program linking failed %s\n", infoLog);
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

#endif
