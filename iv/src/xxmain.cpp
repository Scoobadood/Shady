#include "graph.h"

#include <GLFW/glfw3.h>

GLFWwindow *initgl(){
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);

  // Create window with graphics context
  auto window = glfwCreateWindow(100, 100, "", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  return window;
}


int main( ) {
  auto win = initgl();

  using namespace std;

  auto g = build_graph();

  g.evaluate();

  glfwDestroyWindow(win);
  glfwTerminate();
}