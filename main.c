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

#include "camera.h"
#include "include/cglm/struct/affine-pre.h"
#include "include/cglm/struct/affine.h"
#include "include/cglm/struct/cam.h"
#include "include/cglm/struct/mat4.h"
#include "include/cglm/types-struct.h"
#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow* window);
GLuint generate_texture(const char* filename);

const GLuint SCR_WIDTH = 800;
const GLuint SCR_HEIGHT = 600;

// Camera
Camera camera;
float lastX = (float)SCR_WIDTH / 2;
float lastY = (float)SCR_HEIGHT / 2;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Light
vec3s lightPos = {{1.2f, 1.0f, 2.0f}};

int main(void) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Learn OpenGL", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Failed to create a GLFW window");
    glfwTerminate();

    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glewExperimental = GL_TRUE;
  glewInit();

  // Enables \ Disables
  glEnable(GL_DEPTH_TEST);

  // Shader
  Shader cubeShader =
      shader_create("./glsl/main_vs.glsl", "./glsl/main_fs.glsl");
  Shader lightShader =
      shader_create("./glsl/light_vs.glsl", "./glsl/light_fs.glsl");

  // Square
  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f,
      0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, 0.5f,  -0.5f,
      0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,
      0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f,
      0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};

  // Vertex Buffer
  GLuint VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(VAO);

  // cubePositions
  glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  GLuint lightVAO;
  glGenVertexArrays(1, &lightVAO);
  glBindVertexArray(lightVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  // Triangle End
  camera = create_camerav((vec3s){{0.0f, 0.0f, 3.0f}});

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    process_input(window);

    glClearColor(0x1e / 255.0, 0x29 / 255.0, 0x3b / 255.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    lightPos.x = sin(glfwGetTime()) * 2.0f;
    // lightPos.y = sin(glfwGetTime() / 2) * 1.0f;
    lightPos.z = cos(glfwGetTime() ) * 1.0f;

    shader_use(&cubeShader);
    shader_set_vec3f(&cubeShader, "objectColor", 1.0f, 0.5f, 0.31f);
    shader_set_vec3f(&cubeShader, "lightColor", 1.0f, 1.0f, 1.0f);
    shader_set_vec3(&cubeShader, "lightPos", lightPos);
    shader_set_vec3(&cubeShader, "viewPos", camera.Position);

    mat4s projection =
        glms_perspective(glm_rad(camera.Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);
    mat4s view = camera_get_view_matrix(&camera);
    shader_set_mat4(&cubeShader, "projection", projection);
    shader_set_mat4(&cubeShader, "view", view);

    mat4s model = glms_mat4_identity();
    shader_set_mat4(&cubeShader, "model", model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Lamp
    shader_use(&lightShader);
    shader_set_mat4(&lightShader, "projection", projection);
    shader_set_mat4(&lightShader, "view", view);

    model = glms_mat4_identity();
    model = glms_translate(model, lightPos);
    model = glms_scale_uni(model, 0.2f);
    shader_set_mat4(&lightShader, "model", model);

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteVertexArrays(1, &lightVAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(cubeShader.ID);
  glDeleteProgram(lightShader.ID);

  glfwTerminate();

  return EXIT_SUCCESS;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  (void)window;
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
  (void)window;
  float xpos = (float)xposIn;
  float ypos = (float)yposIn;

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;

    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  camera_process_mouse_movement(&camera, xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  (void)xoffset;
  (void)window;
  camera_process_mouse_scroll(&camera, (float)yoffset);
}

void process_input(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera_process_keyboard(&camera, FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera_process_keyboard(&camera, BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera_process_keyboard(&camera, LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera_process_keyboard(&camera, RIGHT, deltaTime);
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
