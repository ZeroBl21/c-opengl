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
      // positions          // normals           // texture coords
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
      0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
      0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
      -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
      -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
      -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
      -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};

  vec3s cubePositions[] = {
      (vec3s){{0.0f, 0.0f, 0.0f}},    (vec3s){{2.0f, 5.0f, -15.0f}},
      (vec3s){{-1.5f, -2.2f, -2.5f}}, (vec3s){{-3.8f, -2.0f, -12.3f}},
      (vec3s){{2.4f, -0.4f, -3.5f}},  (vec3s){{-1.7f, 3.0f, -7.5f}},
      (vec3s){{1.3f, -2.0f, -2.5f}},  (vec3s){{1.5f, 2.0f, -2.5f}},
      (vec3s){{1.5f, 0.2f, -1.5f}},   (vec3s){{-1.3f, 1.0f, -1.5f}}};

  vec3s pointLightPositions[] = {
      (vec3s){{0.7f, 0.2f, 2.0f}}, (vec3s){{2.3f, -3.3f, -4.0f}},
      (vec3s){{-4.0f, 2.0f, -12.0f}}, (vec3s){{0.0f, 0.0f, -3.0f}}};

  // Vertex Buffer
  GLuint VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(VAO);

  glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float),
                        (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  GLuint lightVAO;
  glGenVertexArrays(1, &lightVAO);
  glBindVertexArray(lightVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  camera = create_camerav((vec3s){{0.0f, 0.0f, 3.0f}});

  GLuint diffuseMap = generate_texture("./textures/container2.png");
  GLuint specularMap = generate_texture("./textures/container2_specular.png");

  shader_use(&cubeShader);
  shader_set_int(&cubeShader, "material.diffuse", 0);
  shader_set_int(&cubeShader, "material.specular", 1);

  // Main Loop
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    process_input(window);

    glClearColor(0x1e / 255.0, 0x29 / 255.0, 0x3b / 255.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_use(&cubeShader);
    shader_set_vec3(&cubeShader, "viewPos", camera.Position);
    shader_set_float(&cubeShader, "material.shininess", 32.0f);

    // Light

    // directional light
    shader_set_vec3f(&cubeShader, "dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader_set_vec3f(&cubeShader, "dirLight.ambient", 0.05f, 0.05f, 0.05f);
    shader_set_vec3f(&cubeShader, "dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    shader_set_vec3f(&cubeShader, "dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    shader_set_vec3(&cubeShader, "pointLights[0].position",
                    pointLightPositions[0]);
    shader_set_vec3f(&cubeShader, "pointLights[0].ambient", 0.05f, 0.05f,
                     0.05f);
    shader_set_vec3f(&cubeShader, "pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    shader_set_vec3f(&cubeShader, "pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    shader_set_float(&cubeShader, "pointLights[0].constant", 1.0f);
    shader_set_float(&cubeShader, "pointLights[0].linear", 0.09f);
    shader_set_float(&cubeShader, "pointLights[0].quadratic", 0.032f);
    // point light 2
    shader_set_vec3(&cubeShader, "pointLights[1].position",
                    pointLightPositions[1]);
    shader_set_vec3f(&cubeShader, "pointLights[1].ambient", 0.05f, 0.05f,
                     0.05f);
    shader_set_vec3f(&cubeShader, "pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    shader_set_vec3f(&cubeShader, "pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    shader_set_float(&cubeShader, "pointLights[1].constant", 1.0f);
    shader_set_float(&cubeShader, "pointLights[1].linear", 0.09f);
    shader_set_float(&cubeShader, "pointLights[1].quadratic", 0.032f);
    // point light 3
    shader_set_vec3(&cubeShader, "pointLights[2].position",
                    pointLightPositions[2]);
    shader_set_vec3f(&cubeShader, "pointLights[2].ambient", 0.05f, 0.05f,
                     0.05f);
    shader_set_vec3f(&cubeShader, "pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    shader_set_vec3f(&cubeShader, "pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    shader_set_float(&cubeShader, "pointLights[2].constant", 1.0f);
    shader_set_float(&cubeShader, "pointLights[2].linear", 0.09f);
    shader_set_float(&cubeShader, "pointLights[2].quadratic", 0.032f);
    // point light 4
    shader_set_vec3(&cubeShader, "pointLights[3].position",
                    pointLightPositions[3]);
    shader_set_vec3f(&cubeShader, "pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    shader_set_vec3f(&cubeShader, "pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    shader_set_vec3f(&cubeShader, "pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    shader_set_float(&cubeShader, "pointLights[3].constant", 1.0f);
    shader_set_float(&cubeShader, "pointLights[3].linear", 0.09f);
    shader_set_float(&cubeShader, "pointLights[3].quadratic", 0.032f);

    // View/Projection
    mat4s projection =
        glms_perspective(glm_rad(camera.Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);
    mat4s view = camera_get_view_matrix(&camera);
    shader_set_mat4(&cubeShader, "projection", projection);
    shader_set_mat4(&cubeShader, "view", view);

    mat4s model = glms_mat4_identity();
    shader_set_mat4(&cubeShader, "model", model);

    // Bind diffuse texture map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    // Bind specular texture map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    glBindVertexArray(VAO);
    for (unsigned int i = 0; i < 10; i++) {
      mat4s model = glms_mat4_identity();
      model = glms_translate(model, cubePositions[i]);
      float angle = 20.0f * i;
      model = glms_rotate(model, glm_rad(angle), (vec3s){{1.0f, 0.3f, 0.5f}});
      shader_set_mat4(&cubeShader, "model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Lamp
    shader_use(&lightShader);
    shader_set_mat4(&lightShader, "projection", projection);
    shader_set_mat4(&lightShader, "view", view);

    glBindVertexArray(lightVAO);
    for (unsigned int i = 0; i < 4; i++) {
      model = glms_mat4_identity();
      model = glms_translate(model, pointLightPositions[i]);
      model = glms_scale_uni(model, 0.2f);
      shader_set_mat4(&lightShader, "model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

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

GLuint generate_texture(const char* path) {
  GLuint texture;
  glGenTextures(1, &texture);

  stbi_set_flip_vertically_on_load(true);
  // load and generate the texture
  int width, height, nrChannels;
  GLubyte* data = stbi_load(path, &width, &height, &nrChannels, 0);
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
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);

  return texture;
}
