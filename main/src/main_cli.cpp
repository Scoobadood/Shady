#include <GLFW/glfw3.h>
#include "xform-io.h"
#include <OpenGL/gl3.h>

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

  auto graph = load_graph("/Users/dave/CLionProjects/image_toys/data/sample_graph.json");
  graph->evaluate();

  glfwDestroyWindow(win);
  glfwTerminate();
}