#include "glfw_support.h"
#include "gl_utils.h"

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <spdlog/spdlog-inl.h>

void initGl() {
  if (!glfwInit()) {
    auto err_msg = "Could not start GLFW3";
    spdlog::error(err_msg);
    throw std::runtime_error(err_msg);
  }
}

GLFWwindow *create_window(const std::string &title, int32_t width, int32_t height) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(),
                                        nullptr, nullptr);
  if (window == nullptr) {
    gl_check_error_and_halt("Failed to create GLFW window");
  }

  glfwMakeContextCurrent(window);
  gl_check_error_and_halt("create window");
  glViewport(0, 0, width, height);
  return window;
}

/**
 *
 */
void init_buffers(GLuint &vao_id, GLuint &vbo_verts, GLuint &vbo_indices) {
  float vertex_data[4 * 4] = {-1.0, -1.0, 0, 0,
                              1.0, -1.0, 1, 0,
                              1.0, 1.0, 1, 1,
                              -1.0, 1.0, 0, 1
  };
  GLushort indices[] = {0, 1, 2, 0, 2, 3};

  glGenVertexArrays(1, &vao_id);
  glGenBuffers(1, &vbo_verts);
  glGenBuffers(1, &vbo_indices);
  glBindVertexArray(vao_id);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_verts);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), &vertex_data[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof (float), 0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof (float), (void*)(2*sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
  gl_check_error_and_halt("init_buffers()");
}

GLuint generate_texture(GLubyte *image_data, GLint texture_width, GLint texture_height) {
  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               texture_width, texture_height, 0,
               GL_RGB, GL_UNSIGNED_BYTE,
               image_data);
  gl_check_error_and_halt("generate_texture");
  free(image_data);
  return texture_id;
}

void tidy_up(GLuint vbo_verts, GLuint vbo_indices, GLuint vao_id, GLuint texture_id, GLFWwindow *window) {
  glDeleteBuffers(1, &vbo_verts);
  glDeleteBuffers(1, &vbo_indices);
  glDeleteVertexArrays(1, &vao_id);
  glDeleteTextures(1, &texture_id);

  glfwDestroyWindow(window);
  glfwTerminate();
}