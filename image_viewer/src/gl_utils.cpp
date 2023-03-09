
#ifdef __APPLE__
// Defined before OpenGL and GLUT includes to avoid deprecation messages
#define GL_SILENCE_DEPRECATION 1
#include <GLFW/glfw3.h>
#endif

#include <spdlog/spdlog-inl.h>

void check_error(const std::string &ctx) {
  auto err = glGetError();
  if (err != GL_NO_ERROR) {
    auto err_msg = fmt::format("{}: Error {}", ctx, err);
    spdlog::error(err_msg);
    throw std::runtime_error(err_msg);
  }
}

GLFWwindow *create_window(const std::string &title, int32_t width, int32_t height) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(),
                                        nullptr, nullptr);
  if (window == nullptr) {
    check_error("Failed to create GLFW window");
  }

  glfwMakeContextCurrent(window);
  check_error("create window");
  glViewport(0, 0, width, height);
  return window;
}

void initGl() {
  if (!glfwInit()) {
    auto err_msg = "Could not start GLFW3";
    spdlog::error(err_msg);
    throw std::runtime_error(err_msg);
  }
}

