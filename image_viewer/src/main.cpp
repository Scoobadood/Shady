#include "float_image.h"
#include "gl_utils.h"

// Shaders
#include "vertex.h"
#include "fragment.h"
#include "shader.h"
#include "image_io.h"
#include "GLFW/glfw3.h"

#include <spdlog/spdlog-inl.h>

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

void initGl() {
  if (!glfwInit()) {
    auto err_msg = "Could not start GLFW3";
    spdlog::error(err_msg);
    throw std::runtime_error(err_msg);
  }
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

void init_buffers(GLuint &vao_id, GLuint &vbo_verts, GLuint &vbo_indices) {
  glm::vec2 vertices[4] = {{0.0, 0.0},
                           {1.0, 0.0},
                           {1.0, 1.0},
                           {0.0, 1.0}
  };
  GLushort indices[] = {0, 1, 2, 0, 2, 3};

  glGenVertexArrays(1, &vao_id);
  glGenBuffers(1, &vbo_verts);
  glGenBuffers(1, &vbo_indices);
  glBindVertexArray(vao_id);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_verts);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
}

int main() {
  initGl();

  auto window = create_window("Image Viewer", 453, 340);

  auto shader = std::unique_ptr<Shader>(new Shader(vertex_shader_source,
                                                   (const GLchar **) nullptr,
                                                   fragment_shader_source));


  GLint texture_width, texture_height;
  auto image_data = load_image("/Users/dave/CLionProjects/image_toys/data/parrot.png",
                               texture_width,
                               texture_height);

  auto texture_id = generate_texture(image_data, texture_width, texture_height);
  glfwSetWindowAspectRatio(window, texture_width, texture_height);

  GLuint vao_id, vbo_verts, vbo_indices;
  init_buffers(vao_id, vbo_verts, vbo_indices);

  shader->use();
  shader->set1i("textureMap", 0);

  int width, height;
  while (!glfwWindowShouldClose(window)) {
    /* Handle retina display */
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteBuffers(1, &vbo_verts);
  glDeleteBuffers(1, &vbo_indices);
  glDeleteVertexArrays(1, &vao_id);
  glDeleteTextures(1, &texture_id);

  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}
