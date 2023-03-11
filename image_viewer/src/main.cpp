#include "float_image.h"
#include "glfw_support.h"
#include "gl_utils.h"

// Shaders
#include "vertex.h"
#include "fragment.h"
#include "shader.h"
#include "image_io.h"

#include <GLFW/glfw3.h>

#include <spdlog/spdlog-inl.h>

struct State {
  float divider;
  bool dragging;
};

void cursor_callback(GLFWwindow *window, double x, double y) {
  auto *config = (State *) glfwGetWindowUserPointer(window);
  if (!config->dragging) return;
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  config->divider = x / width;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
  auto *config = (State *) glfwGetWindowUserPointer(window);
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      config->dragging = true;
    } else if (action == GLFW_RELEASE) {
      config->dragging = false;
    }
  }
}

int main() {
  initGl();

  State state{0.5f, false};

  auto window = create_window("Image Viewer", 453, 340);
  glfwSetWindowUserPointer(window, &state);
  glfwSetCursorPosCallback(window, cursor_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  auto shader = std::unique_ptr<Shader>(new Shader(vertex_shader_source,
                                                   (const GLchar **) nullptr,
                                                   brightness_frag_shader_source));

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

    shader->use();
    shader->set1f("divider", state.divider);

    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  tidy_up(vbo_verts, vbo_indices, vao_id, texture_id, window);

  return EXIT_SUCCESS;
}
