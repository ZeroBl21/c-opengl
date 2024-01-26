#include <GL/glew.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb/stb_image.h"

#define GLFW_DLL
#include <GLFW/glfw3.h>
#define GL_LOG_FILE "gl.log"

#include "include/cglm/struct/affine-pre.h"
#include "include/cglm/struct/affine.h"
#include "include/cglm/struct/mat4.h"
#include "include/cglm/types-struct.h"
#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  (void)window;
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    glClearColor(0x1e / 255.0, 0x29 / 255.0, 0x3b / 255.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
  }
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    puts("Testing 2");
    glClear(GL_COLOR_BUFFER_BIT);
  }
}

GLuint generate_texture(const char* filename) {
  char* ext = strrchr(filename, '.');
  if (ext == NULL || ext == filename) {
    fprintf(stderr, "ERROR: missing file extension");
    return 0;
  }
  ext++;

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound texture
  // object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  int width, height, nrChannels;

  stbi_set_flip_vertically_on_load(true);
  GLubyte* data = stbi_load(filename, &width, &height, &nrChannels, 0);
  if (!data) {
    fprintf(stderr, "ERROR: Failed to load texture");
    return 0;
  }

  if (strncmp(ext, "jpg", 3) == 0) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
  }
  if (strncmp(ext, "png", 3) == 0) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
  }

  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  return texture;
}

int main(void) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "Learn OpenGL", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Failed to create a GLFW window");
    glfwTerminate();

    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glewExperimental = GL_TRUE;
  glewInit();

  // Shader
  Shader shader = shader_create("./glsl/test_vs.glsl", "./glsl/test_fs.glsl");

  // Square
  float vertices[] = {
      // positions        // colors         // texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
      -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
  };

  GLuint indices[] = {
      0, 1, 3,  // first triangle
      1, 2, 3   // second triangle
  };

  // Vertex Buffer
  GLuint VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);
  // Triangle End

  GLuint texture = generate_texture("textures/container.jpg");
  GLuint texture2 = generate_texture("textures/awesomeface.png");

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  shader_use(&shader);
  glUniform1i(glGetUniformLocation(shader.ID, "ourTexture"), 0);
  shader_set_int(&shader, "ourTexture2", 1);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0x1e / 255.0, 0x29 / 255.0, 0x3b / 255.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // Transformations
    mat4s trans = glms_mat4_identity();
    trans = glms_translate(trans, (vec3s){{0.0f, 0.0f, 1.0f}});
    trans =
        glms_rotate(trans, (float)glfwGetTime(), (vec3s){{0.0f, 0.0f, 1.0f}});
    shader_set_mat4(&shader, "transform", trans);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Second Container
    mat4s transform = glms_mat4_identity();  // reset it to identity matrix
    transform = glms_translate(transform, (vec3s){{-0.5f, 0.5f, 0.0f}});
    float scaleAmount = (float)(sin(glfwGetTime()));
    transform =
        glms_scale(transform, (vec3s){{scaleAmount, scaleAmount, scaleAmount}});
    shader_set_mat4(&shader, "transform", transform);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shader.ID);

  glfwTerminate();

  return EXIT_SUCCESS;
}
