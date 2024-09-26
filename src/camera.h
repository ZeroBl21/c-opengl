#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <stdbool.h>

#include "cglm/types-struct.h"

typedef enum { FORWARD, BACKWARD, LEFT, RIGHT } CameraMovement;

extern const float YAW;
extern const float PITCH;
extern const float SPEED;
extern const float SENSITIVITY;
extern const float ZOOM;

typedef struct {
  vec3s Position;
  vec3s Front;
  vec3s Up;
  vec3s Right;
  vec3s WorldUp;
  // Euler Angles
  float Yaw;
  float Pitch;
  // Camera Options
  float MovementSpeed;
  float MouseSensitivity;
  float Zoom;
} Camera;

Camera create_camera(vec3s position, vec3s up, float yaw, float pitch);
Camera create_camerav(vec3s position);
Camera create_camera_scalar(float posX, float posY, float posZ, float upX,
                            float upY, float upZ, float yaw, float pitch);
mat4s camera_get_view_matrix(Camera *camera);
void camera_process_keyboard(Camera *camera, CameraMovement direction,
                             float deltaTime);
void camera_process_mouse_movement(Camera *camera, float xoffset, float yoffset,
                                   bool constraintPitch);
void camera_process_mouse_scroll(Camera *camera, float yoffset);

#endif
