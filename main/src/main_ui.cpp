#include "xform-io.h"
#include "xform-texture-meta.h"
#include "xform-factory.h"

#include "ui_state.h"
#include "ui_theme.h"
#include "ui_menu.h"
#include "ui_graph.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <spdlog/cfg/env.h>


GLFWwindow *initgl() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Create window with graphics context
  auto window = glfwCreateWindow(900, 620, "Shady!", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync
  return window;
}

void initImGui(GLFWwindow *window) {
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
  const char *glsl_version = "#version 410";
  ImGui_ImplOpenGL3_Init(glsl_version);
}

int main(int argc, const char *argv[]) {
  spdlog::cfg::load_env_levels();

  // Init glfw
  auto glfw_window = initgl();

  // Init ImGui
  initImGui(glfw_window);

  // State
  State state;

  state.graph = nullptr;
  bool open_graph_menu_open = false;
  if (argc == 2) {
    state.graph = load_graph(argv[1]);
    if( state.graph != nullptr) {
      state.graph_file_name = argv[1];
      state.graph->evaluate();
    }
  }

  auto theme = Theme::theme();
  init_global_style();


  // Loop forever
  while (!glfwWindowShouldClose(glfw_window)) {
    glfwPollEvents();

    // Cls - no depth here.
    glClearColor(30 / 255.0f, 30 / 255.0f, 30 / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Start defining the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // If there's a graph, render it.
    if (state.graph) {
      render_graph(state, theme);
    }

    /* Menus */
    do_menus(open_graph_menu_open, state);

    if (ImGui::BeginPopupContextVoid("Add")) {
      for (auto &xform_name: XformFactory::registered_types()) {
        if (ImGui::Selectable(xform_name.c_str())) {
          auto xf = XformFactory::make_xform(xform_name);
          state.graph->add_xform(xf);
        }
      }
      ImGui::EndPopup();
    }

    /* UI Tweaking */
    if (ImGui::Begin("State")) {
      ImGui::Text("Connecting output : %s", state.connecting_output ? state.connecting_output->name().c_str() : "null");
      ImGui::Text("Connecting input : %s", state.connecting_input ? state.connecting_input->name().c_str() : "null");
      ImGui::Text("Connecting xform : %s", state.connecting_xform ? state.connecting_xform->name().c_str() : "null");
    }
    ImGui::End();

    // Do actual rendering of frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    int display_width, display_height;
    glfwGetFramebufferSize(glfw_window, &display_width, &display_height);
    glViewport(0, 0, display_width, display_height);
    glfwSwapBuffers(glfw_window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(glfw_window);
  glfwTerminate();

  return EXIT_SUCCESS;
}
