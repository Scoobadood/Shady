#include "xform-io.h"
#include "xform-graph.h"
#include "file_utils.h"
#include "xform-texture-meta.h"

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

void render_graph(const std::shared_ptr<XformGraph> &graph) {
  std::map<std::pair<std::string, std::string>, ImVec2> in_port_coords;
  std::map<std::pair<std::string, std::string>, ImVec2> out_port_coords;

  // For each node, make a box and add a name to it.
  for (const auto &xform: graph->xforms()) {
    ImGui::Begin(xform->name().c_str(), nullptr,
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_AlwaysAutoResize |
                 ImGuiWindowFlags_NoCollapse
    );

    ImGui::SetWindowSize(ImVec2((float) 160, (float) 140));
    auto has_op = !xform->output_port_descriptors().empty();
    if (has_op) {
      if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_None)) {
        /* Add image */
        auto op = graph->output_from(xform->name(), xform->output_port_descriptors().front()->name());
        if (op) {
          auto tx = (TextureMetadata *) op.get();
          ImGui::Image((ImTextureID) (tx->texture_id), ImVec2(160, 120));
        }
      }
    }
    auto has_config = !xform->config().descriptors().empty();
    if (has_config) {
      if (ImGui::CollapsingHeader("Config", ImGuiTreeNodeFlags_None)) {
        /* Add image */
        for (const auto &pd: xform->config().descriptors())
          ImGui::Text(pd.name.c_str());
      }
    }

    /*
     * Render inputs
     */
    auto num_inputs = xform->input_port_descriptors().size();
    ImVec2 window_size = ImGui::GetWindowSize();
    auto spacing = window_size.y / (float) (num_inputs + 1);
    ImVec2 window_pos = ImGui::GetWindowPos();
    ImVec2 port_pos = {window_pos.x, window_pos.y + spacing};
    for (const auto &ipd: xform->input_port_descriptors()) {
      ImGui::GetForegroundDrawList()->AddCircleFilled(port_pos,
                                                      5.0f, IM_COL32(255, 0, 0, 255),
                                                      0);
      in_port_coords.emplace(std::make_pair(xform->name(), ipd->name()),
                             port_pos);

      auto sz = ImGui::CalcTextSize(ipd->name().c_str());
      ImGui::GetForegroundDrawList()->AddText({port_pos.x + 5.0f, port_pos.y - sz.y * 0.5f}, IM_COL32_BLACK,
                                              ipd->name().c_str());
      port_pos.y += spacing;
    }

    /*
     * Render outputs
     */
    auto num_outputs = xform->output_port_descriptors().size();
    spacing = window_size.y / (float) (num_outputs + 1);
    port_pos = {window_pos.x + window_size.x, window_pos.y + spacing};
    for (const auto &opd: xform->output_port_descriptors()) {
      ImGui::GetForegroundDrawList()->AddCircleFilled(port_pos,
                                                      5.0f, IM_COL32(0, 255, 0, 255),
                                                      0);
      out_port_coords.emplace(std::make_pair(xform->name(), opd->name()),
                              port_pos);
      auto sz = ImGui::CalcTextSize(opd->name().c_str());
      ImGui::GetForegroundDrawList()->AddText({window_pos.x + window_size.x - sz.x - 5.0f, port_pos.y - sz.y * 0.5f},
                                              IM_COL32_BLACK, opd->name().c_str());
      port_pos.y += spacing;
    }

    ImGui::End();
//    ImGui::SetNextWindowPos();

  }

  /**
   * Render connections
   */
  for (const auto &conn: graph->connections()) {
    ImVec2 from = out_port_coords.at(conn.first);
    ImVec2 to = in_port_coords.at(conn.second);
    auto midX = from.x + (to.x - from.x) / 2.0f;
    ImGui::GetBackgroundDrawList()->AddBezierCubic(from,
                                                   ImVec2(midX, from.y),
                                                   ImVec2(midX, to.y),
                                                   to,
                                                   IM_COL32(80, 60, 0, 255), 2);
  }
}

char **get_graph_file_names(uint32_t &num_files) {
  std::vector<std::string> files;
  files_in_directory(get_cwd(), files, [](const char *const file_name) {
    auto l = strlen(file_name);
    if (l < 6) return false; // too short
    if (file_name[l - 5] != '.') return false;
    if (file_name[l - 4] != 'j' && file_name[l - 4] != 'J') return false;
    if (file_name[l - 3] != 's' && file_name[l - 3] != 'S') return false;
    if (file_name[l - 2] != 'o' && file_name[l - 2] != 'O') return false;
    if (file_name[l - 1] != 'n' && file_name[l - 1] != 'N') return false;
    return true;
  });

  num_files = files.size();
  if (num_files) {
    char **names = new char *[files.size()];
    for (auto i = 0; i < num_files; ++i) {
      names[i] = new char[files[i].size() + 1];
      strcpy(names[i], files[i].c_str());
    }
    return names;
  }
  return nullptr;
}

void do_menus(std::shared_ptr<XformGraph> &graph, bool &open_graph_menu_open) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Graph")) {
      if (ImGui::MenuItem("Open...", "Ctrl+O")) {
        open_graph_menu_open = true;
      }
      if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
      if (ImGui::MenuItem("Close", "Ctrl+W")) { graph = nullptr; }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (open_graph_menu_open)
    ImGui::OpenPopup("XXX");

  if (ImGui::BeginPopupModal("XXX")) {
    ImGui::Text("Open a graph");
    // Testing behavior of widgets stacking their own regular popups over the modal.
    static int selected_item = 0;
    uint32_t num_graph_files;
    auto graph_file_names = get_graph_file_names(num_graph_files);
    ImGui::ListBox("Graphs", &selected_item, graph_file_names, num_graph_files);

    if (ImGui::Button("Cancel")) {
      open_graph_menu_open = false;
      ImGui::CloseCurrentPopup();
    }
    if (num_graph_files == 0)
      ImGui::BeginDisabled();
    if (ImGui::Button("Open")) {
      open_graph_menu_open = false;
      graph = load_graph(graph_file_names[selected_item]);
      graph->evaluate();
      ImGui::CloseCurrentPopup();
    }
    if (num_graph_files == 0)
      ImGui::EndDisabled();
    ImGui::EndPopup();

    for (auto i = 0; i < num_graph_files; ++i) {
      delete[] graph_file_names[i];
    }
    delete graph_file_names;
  }
}

int main() {
  spdlog::cfg::load_env_levels();

  auto window = initgl();

  initImGui(window);

  std::shared_ptr<XformGraph> graph = nullptr;

  bool open_graph_menu_open = false;
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    /*
     * Start the Dear ImGui frame
     */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (graph) {
      render_graph(graph);
    }

    /* Menus */
    do_menus(graph, open_graph_menu_open);


/*
 * Do actual rendering
 */
    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h
    );
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
