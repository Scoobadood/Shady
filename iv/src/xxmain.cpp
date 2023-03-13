#include "load-file-xform.h"
#include <GLFW/glfw3.h>

int main( ) {
  glfwInit();
  const char *glsl_version = "#version 410";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);

  // Create window with graphics context
  GLFWwindow *window = glfwCreateWindow(100, 100, "", NULL, NULL);

  auto lof = LoadFileXform();
  lof.config().set("file_name", "/Users/dave/CLionProjects/image_toys/data/parrot.png");
  auto out = lof.apply({});
  glfwDestroyWindow(window);
  glfwTerminate();
}