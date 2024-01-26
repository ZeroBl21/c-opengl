#include <GL/glew.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define GLFW_DLL
#include <GLFW/glfw3.h>
#define GL_LOG_FILE "gl.log"

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

int main(void) {
  puts("hello World");
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
      // positions        // colors
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom left
      0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f   // top
  };

  // float vertices[] = {
  //     // positions        // colors
  //     0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
  //     -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom left
  //     0.0f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f   // top
  // };

  // Vertex Buffer
  GLuint VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);
  // Triangle End

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0x1e / 255.0, 0x29 / 255.0, 0x3b / 255.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    shader_use(&shader);
    shader_set_float(&shader, "positionOffset", 0.0f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shader.ID);

  glfwTerminate();

  return EXIT_SUCCESS;
}
