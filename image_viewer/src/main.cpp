#include "float_image.h"
#include "glfw_support.h"
#include "gl_utils.h"

// Shaders
#include "vertex.h"
#include "fragment.h"
#include "shader.h"
#include "image_io.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include <spdlog/spdlog-inl.h>

struct State {
  GLuint texture_id;
  GLuint tgt_texture_id;
  std::unique_ptr<Shader> shader;
  float texture_width;
  float texture_height;
  float divider;
  bool dragging;
};

void main_loop(GLFWwindow *window, State &state);

void glfw_error_callback(int error_code, const char *description) {
  spdlog::error("GLFW Error {}: {}", error_code, description);
}

GLuint load_parrot_to_texture(GLint &texture_width, GLint &texture_height) {
  auto image_data = load_image("/Users/dave/CLionProjects/image_toys/data/parrot.png",
                               texture_width,
                               texture_height);
  auto texture_id = generate_texture(image_data, texture_width, texture_height);
  return texture_id;
}

int main() {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return EXIT_FAILURE;

  /*
   * Create the window root window
   */
  const char *glsl_version = "#version 410";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Create window with graphics context
  GLFWwindow *window = glfwCreateWindow(1280, 720, "Image Toy", NULL, NULL);
  if (window == nullptr)
    return EXIT_FAILURE;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void) io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);


  GLint texture_width, texture_height;
  auto parrot_txid = load_parrot_to_texture(texture_width, texture_height);

  State state{parrot_txid,
              0,
              nullptr,
              (float) texture_width,
              (float) texture_height,
              0.5f, false};


  state.shader = std::unique_ptr<Shader>(new Shader(vertex_shader_source,
                                                   (const GLchar **) nullptr,
                                                   brightness_frag_shader_source));
  while (!glfwWindowShouldClose(window)) {
    main_loop(window, state);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}

void render_parrot_to_texture(State & state){

  unsigned int fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  // Make and bind texture
  glGenTextures(1, &state.tgt_texture_id);
  glBindTexture(GL_TEXTURE_2D, state.tgt_texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               state.texture_width, state.texture_height,
               0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // Unbind generated texture
  glBindTexture(GL_TEXTURE_2D, 0);
  // Attach to FBO
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, state.tgt_texture_id, 0);
  GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, drawBuffers);
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    throw std::runtime_error("FB not complete");

  /* Render cycle */
  GLuint vao_id, vbo_verts, vbo_indices;
  init_buffers(vao_id, vbo_verts, vbo_indices);
  glBindTexture(GL_TEXTURE_2D, state.texture_id);
  glBindVertexArray(vao_id);

  glViewport(0,0,state.texture_width, state.texture_height);
  glClearColor(.6,.4,.2,1.);
  glClear(GL_COLOR_BUFFER_BIT);
  state.shader->use();
  state.shader->set1f("divider", state.divider);
  state.shader->set1i("textureMap", 0);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
  gl_check_error_and_halt("draw elements");

  // Unbind
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  gl_check_error_and_halt("Rendered");

  glDeleteFramebuffers(1, &fbo);
}


/**
 * Main processing loop
 */
void main_loop(GLFWwindow * window, State &state) {
  glfwPollEvents();

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // Add parrot image
  render_parrot_to_texture(state);
  ImGui::Begin("Parrot", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
  ImGui::Image((void *) state.texture_id, ImVec2(state.texture_width, state.texture_height));
  ImGui::End();

  ImGui::Begin("Bright Parrot", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
  ImGui::Image((void *) state.tgt_texture_id, ImVec2(state.texture_width, state.texture_height));
  ImGui::End();

  // Rendering
  ImGui::Render();
  int display_w, display_h;
  glfwGetFramebufferSize(window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(window);
}