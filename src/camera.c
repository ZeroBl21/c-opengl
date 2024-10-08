#include "camera.h"

#include "cglm/struct/cam.h"

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 80.0f;

// Privates.
static void update_camera_vectors(Camera *camera);

// Creates a camera with specified initial parameters and returns it.
Camera create_camera(vec3s position, vec3s up, float yaw, float pitch) {
    Camera camera = {
        .Position = position,
        .Front = (vec3s){{0.0f, 0.0f, -1.0f}},
        .WorldUp = up,
        .Yaw = yaw,
        .Pitch = pitch,
        .MovementSpeed = SPEED,
        .MouseSensitivity = SENSITIVITY,
        .Zoom = ZOOM,
    };

    update_camera_vectors(&camera);

    return camera;
}

// Creates a camera with default orientation, using the specified position.
Camera create_camerav(vec3s position) {
    return create_camera(position, (vec3s){{0.0f, 1.0f, 0.0f}}, YAW, PITCH);
}

// Creates a camera with specified initial parameters using scalar values.
Camera create_camera_scalar(float posX, float posY, float posZ, float upX,
                            float upY, float upZ, float yaw, float pitch) {
    Camera camera = {
        .Position = (vec3s){{posX, posY, posZ}},
        .Front = (vec3s){{0.0f, 0.0f, -1.0f}},
        .WorldUp = (vec3s){{upX, upY, upZ}},
        .Yaw = yaw,
        .Pitch = pitch,
        .MovementSpeed = SPEED,
        .MouseSensitivity = SENSITIVITY,
        .Zoom = ZOOM,
    };

    update_camera_vectors(&camera);

    return camera;
}

// Calculates and returns the view matrix for the given camera.
// returns the view matrix calculated using Euler Angles and the LookAt Matrix
mat4s camera_get_view_matrix(Camera *camera) {
    return glms_lookat(camera->Position,
                       glms_vec3_add(camera->Position, camera->Front),
                       camera->Up);
}

// processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
void camera_process_keyboard(Camera *camera, CameraMovement direction,
                             float deltaTime) {
    float velocity = camera->MovementSpeed * deltaTime;
    if (direction == FORWARD) {
        camera->Position = glms_vec3_muladds(camera->Front, velocity, camera->Position);
    }
    if (direction == BACKWARD)
        camera->Position = glms_vec3_mulsubs(camera->Front, velocity, camera->Position);
    if (direction == LEFT)
        camera->Position = glms_vec3_mulsubs(camera->Right, velocity, camera->Position);
    if (direction == RIGHT)
        camera->Position = glms_vec3_muladds(camera->Right, velocity, camera->Position);
}

// Processes mouse movement to update the camera's orientation.
void camera_process_mouse_movement(Camera *camera, float xoffset,
                                   float yoffset, bool constraintPitch) {
    xoffset *= camera->MouseSensitivity;
    yoffset *= camera->MouseSensitivity;

    camera->Yaw += xoffset;
    camera->Pitch += yoffset;

    if (constraintPitch) {
        if (camera->Pitch > 89.0f) camera->Pitch = 89.f;
        if (camera->Pitch < -89.0f) camera->Pitch = -89.f;
    }

    update_camera_vectors(camera);
}

// Processes mouse scroll to update the camera's zoom level.
void camera_process_mouse_scroll(Camera *camera, float yoffset) {
    camera->Zoom -= (float)yoffset;
    if (camera->Zoom < 1.0f) camera->Zoom = 1.0f;
    if (camera->Zoom > 80.0f) camera->Zoom = 80.0f;
}

// ------------------------------------------------------------------------

// Updates the camera's internal vectors based on its orientation.
// Calculates the front vector from the Camera's (updated) Euler Angles
static void update_camera_vectors(Camera *camera) {
    vec3s front = {
        .x = cos(glm_rad(camera->Yaw)) * cos(glm_rad(camera->Pitch)),
        .y = sin(glm_rad(camera->Pitch)),
        .z = sin(glm_rad(camera->Yaw)) * cos(glm_rad(camera->Pitch)),
    };
    camera->Front = glms_normalize(front);
    camera->Right = glms_normalize(glms_cross(camera->Front, camera->WorldUp));
    camera->Up = glms_normalize(glms_cross(camera->Right, camera->Front));
}
