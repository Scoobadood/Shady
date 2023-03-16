#include <GLFW/glfw3.h>
#include "xform-io.h"
#include <OpenGL/gl3.h>
#include <spdlog/spdlog-inl.h>

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


int main( int argc, const char * argv[]) {
  auto win = initgl();

  using namespace std;

  if( argc != 2 ) {
    spdlog::info("Nothing to do");
    return EXIT_FAILURE;
  }
  auto graph = load_graph(argv[1]);
  graph->evaluate();

  glfwDestroyWindow(win);
  glfwTerminate();
  return EXIT_SUCCESS;
}