#include "xform-io.h"
#include "command.h"

#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <spdlog/spdlog-inl.h>
#include <iostream>
#include <strstream>

GLFWwindow *initgl() {
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

int main(int argc, const char *argv[]) {
  auto win = initgl();

  using namespace std;

  map<string, shared_ptr<void>> context;
  string cmd;
  while (true) {
    cout << "> ";
    getline(cin, cmd);
    auto command = Command::from_string(cmd);
    if( !command ) {
      cout << "Unrecognised command" << endl;
      continue;
    }
    if( !command->is_valid()) {
      cout << command->err_msg() << endl;
      continue;
    }

    if( int err = command->execute(context) ) {
      cout << "Error :" << err << endl;
    }
    cout << command->output() << endl;

    if( context["should_quit"] ) break;
  }

  glfwDestroyWindow(win);
  glfwTerminate();
  return EXIT_SUCCESS;
}