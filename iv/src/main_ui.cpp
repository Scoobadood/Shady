#include "graph.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

GLFWwindow *initgl() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Create window with graphics context
  auto window = glfwCreateWindow(1280, 720, "Graph Toy", nullptr, nullptr);
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

int main() {
  auto window = initgl();

  initImGui(window);

  auto graph = build_graph();
  graph.evaluate();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    /*
     * Start the Dear ImGui frame
     */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    /*
     * Render the graph
     */

    // For each node, make a box and add a name to it.
    for (auto &xform: graph.xforms()) {
      ImGui::Begin(xform->name().c_str(), nullptr,
                   ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoCollapse
      );
      ImGui::SetWindowSize(ImVec2((float) 160, (float) 120));

      /*
       * Render inputs
       */
      auto num_inputs = xform->input_port_descriptors().size();
      ImVec2 window_size = ImGui::GetWindowSize();
      auto spacing = window_size.y / (num_inputs + 1);
      ImVec2 window_pos = ImGui::GetWindowPos();
      ImVec2 port_pos = {window_pos.x, window_pos.y + spacing};
      for (const auto &ipd: xform->input_port_descriptors()) {
        ImGui::GetForegroundDrawList()->AddCircleFilled(port_pos,
                                                        5.0f, IM_COL32(255, 0, 0, 255),
                                                        0);
        port_pos.y += spacing;
      }

      /*
       * Render outputs
       */
      auto num_outputs = xform->output_port_descriptors().size();
      spacing = window_size.y / (num_outputs + 1);
      port_pos = {window_pos.x + window_size.x, window_pos.y + spacing};
      for( const auto & opd : xform->output_port_descriptors()) {
        ImGui::GetForegroundDrawList()->AddCircleFilled(port_pos,
                                                        5.0f, IM_COL32(0, 255, 0, 255),
                                                        0);
        port_pos.y += spacing;
      }

      ImGui::End();
    }

    /*
     * Do actual rendering
     */
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}
