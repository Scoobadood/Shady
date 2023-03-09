#include <iostream>

#include "float_image.h"
#include "gl_utils.h"


int main() {
  initGl();

  auto window = create_window("Image Viewer", 800, 600);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}
