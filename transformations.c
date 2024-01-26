#include <stdio.h>
#include <stdlib.h>

#include "include/cglm/struct/affine-pre.h"
#include "include/cglm/struct/mat4.h"
#include "include/cglm/types-struct.h"

int main(void) {
  vec4s testing = {{1.0f, 0.0f, 0.0f, 1.0f}};
  mat4s trans = glms_mat4_identity();

  trans = glms_translate(trans, (vec3s){{1.0f, 1.0f, 0.0f}});
  vec4s result = glms_mat4_mulv(trans, testing);

  printf("x: %.2f, y: %.2f, z: %.2f\n", result.x, result.y, result.z);

  return EXIT_SUCCESS;
}
