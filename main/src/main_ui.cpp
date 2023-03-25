#include "xform-io.h"
#include "xform-graph.h"
#include "file_utils.h"
#include "xform-texture-meta.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <spdlog/cfg/env.h>

float g_rounding = 5.0f;
ImVec4 g_xform_bg_colour = ImColor(40, 40, 40, 255);
ImVec4 g_title_bg_colour = ImColor(57, 57, 57, 255);
ImVec4 g_text_colour = ImColor(140, 140, 140, 255);
ImVec4 g_in_port_bg_colour = ImColor(57, 57, 57, 255);
ImVec4 g_out_port_bg_colour = ImColor(43, 43, 43, 255);
ImColor g_conn_in_port_colour = ImColor(170, 230, 150, 255);
ImColor g_conn_out_port_colour = ImColor(230, 170, 150, 255);


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

/*
 * Render input ports for a specific Xform
 */
void render_input_ports(const std::string &xform_name,
                        const std::vector<std::shared_ptr<const InputPortDescriptor>> &input_port_descriptors,
                        const std::vector<bool> &is_connected,
                        std::map<std::pair<std::string, std::string>, ImVec2> &in_port_coords,
                        int &current_selection) {

  // Compute some metrics
  auto line_height = ImGui::GetTextLineHeightWithSpacing();
  auto num_inputs = input_port_descriptors.size();
  auto window_x = ImGui::GetWindowPos().x;
  auto window_y = ImGui::GetWindowPos().y;
  auto port_radius = 0.3f * line_height;
  auto indent = (port_radius + 2.0f) * 3;

  ImGui::PushStyleColor(ImGuiCol_ChildBg, g_in_port_bg_colour);


  if (ImGui::BeginChild("##inputs", {ImGui::GetWindowWidth() * 0.5f, 0})) {

    ImGui::Indent(indent);
    for (int ipd_idx = 0; ipd_idx < num_inputs; ipd_idx++) {
      const auto &ipd = input_port_descriptors.at(ipd_idx);

      const bool is_selected = (current_selection == ipd_idx);
      const bool is_port_connected = is_connected.at(ipd_idx);

      /* Render the port */
      auto port_pos = ImVec2{window_x + indent * 0.5f, window_y + (ipd_idx + 0.5f) * line_height};
      if (is_port_connected) {
        ImGui::GetWindowDrawList()->AddCircleFilled(port_pos,
                                                    port_radius, g_conn_in_port_colour);
      } else {
        ImGui::GetWindowDrawList()->AddCircle(port_pos,
                                              port_radius, g_conn_in_port_colour);
      }
      in_port_coords.emplace(std::make_pair(xform_name, ipd->name()), port_pos);

      // And the port name
      if (ImGui::Selectable(ipd->name().c_str(), is_selected)) {
        current_selection = ipd_idx;
      }

      // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
      if (is_selected) {
        ImGui::SetItemDefaultFocus();
      }

    }
    ImGui::Unindent(indent);
  }
  ImGui::EndChild();

  ImGui::PopStyleColor(1);
}

/*
 * Render input ports for a specific Xform
 */
void render_output_ports(const std::string &xform_name,
                         const std::vector<std::shared_ptr<const OutputPortDescriptor>> &output_port_descriptors,
                         const std::vector<bool> &is_connected,
                         std::map<std::pair<std::string, std::string>, ImVec2> &out_port_coords,
                         int &current_selection) {
  auto num_outputs = output_port_descriptors.size();

  // Compute some metrics
  auto line_height = ImGui::GetTextLineHeightWithSpacing();
  auto window_r = ImGui::GetWindowPos().x + ImGui::GetWindowWidth();
  auto window_y = ImGui::GetWindowPos().y;
  auto port_radius = 0.3f * line_height;
  auto indent = (port_radius + 2.0f) * 3;

  ImGui::PushStyleColor(ImGuiCol_ChildBg, g_out_port_bg_colour);
  ImGui::PushStyleColor(ImGuiCol_Border, g_xform_bg_colour);

  if (ImGui::BeginChild("##outputs", {ImGui::GetWindowWidth() * 0.5f, 0})) {

    for (int opd_idx = 0; opd_idx < num_outputs; opd_idx++) {
      const auto &opd = output_port_descriptors.at(opd_idx);

      const bool is_selected = (current_selection == opd_idx);
      const bool is_port_connected = is_connected.at(opd_idx);

      /* Render the port */
      auto port_pos = ImVec2{window_r - (indent * 0.5f), window_y + (opd_idx + 0.5f) * line_height};
      if (is_port_connected) {
        ImGui::GetWindowDrawList()->AddCircleFilled(port_pos, port_radius, g_conn_out_port_colour);
      } else {
        ImGui::GetWindowDrawList()->AddCircle(port_pos, port_radius, g_conn_out_port_colour);
      }
      out_port_coords.emplace(std::make_pair(xform_name, opd->name()), port_pos);

      if (ImGui::Selectable(opd->name().c_str(), is_selected, 0,
                            {ImGui::GetWindowWidth() - indent, 0})) {
        current_selection = opd_idx;
      }

      // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
      if (is_selected) {
        ImGui::SetItemDefaultFocus();
      }
    }
  }
  ImGui::EndChild();
  ImGui::PopStyleColor(2);
}

/*
 * Render input and output ports if they are present.
 */
void render_ports(const std::shared_ptr<XformGraph> &graph,
                  const std::shared_ptr<const Xform> &xform,
                  std::map<std::pair<std::string, std::string>, ImVec2> &in_port_coords,
                  std::map<std::pair<std::string, std::string>, ImVec2> &out_port_coords
) {
  auto num_input_ports = xform->input_port_descriptors().size();
  auto num_output_ports = xform->output_port_descriptors().size();
  auto max_ports = std::max(num_input_ports, num_output_ports);

  // Build UI details
  auto height = ImGui::GetTextLineHeightWithSpacing() * (float) max_ports + (ImGui::GetStyle().ChildBorderSize * 2);

  ImGui::BeginChild("##ports", {0, height}, true);

  int selected_input = 0;
  std::vector<bool> inputs_connected;
  inputs_connected.reserve(num_input_ports);
  for (auto ip_idx = 0; ip_idx < num_input_ports; ++ip_idx) {
    auto ipd = xform->input_port_descriptors().at(ip_idx);
    inputs_connected.push_back(graph->input_is_connected(xform->name(), ipd->name()));
  }
  render_input_ports(xform->name(), xform->input_port_descriptors(),
                     inputs_connected, in_port_coords, selected_input);

  ImGui::SameLine(0, 0);

  int selected_output = 0;
  std::vector<bool> outputs_connected;
  outputs_connected.reserve(num_output_ports);
  for (auto op_idx = 0; op_idx < num_output_ports; ++op_idx) {
    auto opd = xform->output_port_descriptors().at(op_idx);
    outputs_connected.push_back(graph->output_is_connected(xform->name(), opd->name()));
  }

  render_output_ports(xform->name(), xform->output_port_descriptors(),
                      outputs_connected, out_port_coords, selected_output);
  ImGui::EndChild();

}

/*
 * Render an individual transform.
 * Each transform has
 * - a name
 * - zero or more input ports (with type)
 * - zero or more output ports (with type)
 * - an optional configuration
 * - an optionally rendered output
 */
void render_xform(const std::shared_ptr<XformGraph> &graph,
                  const std::shared_ptr<const Xform> &xform,
                  std::map<std::pair<std::string, std::string>, ImVec2> &in_port_coords,
                  std::map<std::pair<std::string, std::string>, ImVec2> &out_port_coords
) {
  // TODO: Work out dynamic sizing later
  ImGui::SetNextWindowSize(ImVec2(190.0f, 0));

  /* Own window per xform */
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
  ImGui::Begin(xform->name().c_str(), nullptr,
               ImGuiWindowFlags_NoResize
               | ImGuiWindowFlags_AlwaysAutoResize
               | ImGuiWindowFlags_NoCollapse
  );

  render_ports(graph, xform, in_port_coords, out_port_coords);

  /* Image output - the selected output port is rendered */
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
        ImGui::Text("%s", pd.name.c_str());
    }
  }

  ImGui::End();
  ImGui::PopStyleVar(1);
}

/*
 * Render the graph by rendering the nodes.
 */
void render_graph(const std::shared_ptr<XformGraph> &graph) {
  using namespace std;

  /*
   * Each node is a window that controls its own port locations.
   * These maps get populated as nodes are drawn and then connections
   * are drawn separately.
   */
  map<pair<string, string>, ImVec2> in_port_coords;
  map<pair<string, string>, ImVec2> out_port_coords;

  static struct State {
    bool connecting = false;
    std::string conn_xform;
    std::shared_ptr<InputPortDescriptor> conn_ipd;
    ImVec2 conn_position;
  } state;

  /* For each xform in the graph, make a node
   */
  for (const auto &xform: graph->xforms()) {
    render_xform(graph, xform, in_port_coords, out_port_coords);
  }

  /*
   * Render connections between the nodes
   */
  for (const auto &conn: graph->connections()) {
    auto from_it = out_port_coords.find(conn.first);
    auto to_it = in_port_coords.find(conn.second);
    if (from_it != out_port_coords.end() && to_it != in_port_coords.end()) {
      ImVec2 from = from_it->second;
      ImVec2 to = to_it->second;
      auto mid_x = from.x + (to.x - from.x) / 2.0f;
      ImGui::GetBackgroundDrawList()->AddBezierCubic(from,
                                                     ImVec2(mid_x, from.y),
                                                     ImVec2(mid_x, to.y),
                                                     to,
                                                     IM_COL32(80, 60, 0, 255), 2);
    }
  }

  if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    state.connecting = true;
    state.conn_position = ImGui::GetMousePos();
  }

  if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
    state.connecting = false;
  }

  if (state.connecting) {//} ImGui::IsMouseDragging(ImGuiMouseButton_Left, 2)) {
    auto delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 2);
    if (delta.x != 0.0 && delta.y != 0.0) {
      ImVec2 to{state.conn_position.x + delta.x, state.conn_position.y + delta.y};
      auto mid_x = state.conn_position.x + delta.x * 0.5f;
      ImGui::GetBackgroundDrawList()->AddBezierCubic(state.conn_position,
                                                     ImVec2(mid_x, state.conn_position.y),
                                                     ImVec2(mid_x, to.y),
                                                     to,
                                                     IM_COL32(128, 128, 128, 255), 2);
    }
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

    // Disable iOpen button if no files
    if (num_graph_files == 0)
      ImGui::BeginDisabled();

    if (ImGui::Button("Open")) {
      open_graph_menu_open = false;
      if (graph_file_names) {
        graph = load_graph(graph_file_names[selected_item]);
        if (graph) {
          graph->evaluate();
          ImGui::CloseCurrentPopup();
        }
      }
    }

    if (num_graph_files == 0)
      ImGui::EndDisabled();

    ImGui::EndPopup();

    if (graph_file_names) {
      for (auto i = 0; i < num_graph_files; ++i) {
        delete[] graph_file_names[i];
      }
      delete graph_file_names;
    }
  }
}

void set_global_style() {
  auto &style = ImGui::GetStyle();
  style.WindowRounding = g_rounding;
  style.Colors[ImGuiCol_TitleBg] = g_title_bg_colour;
  style.Colors[ImGuiCol_TitleBgActive] = g_title_bg_colour;
  style.Colors[ImGuiCol_Text] = g_text_colour;
  style.Colors[ImGuiCol_WindowBg] = g_xform_bg_colour;
  style.Colors[ImGuiCol_ChildBg] = g_xform_bg_colour;
}

int main(int argc, const char *argv[]) {
  spdlog::cfg::load_env_levels();

  // Init glfw
  auto glfw_window = initgl();

  // Init ImGui
  initImGui(glfw_window);

  // State
  std::shared_ptr<XformGraph> graph = nullptr;
  bool open_graph_menu_open = false;
  if (argc == 2) {
    graph = load_graph(argv[1]);
  }

  // Loop forever
  while (!glfwWindowShouldClose(glfw_window)) {
    glfwPollEvents();

    // Cls - no depth here.
    glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    set_global_style();

    // Start defining the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // If there's a graph, render it.
    if (graph) {
      render_graph(graph);
    }

    /* Menus */
    do_menus(graph, open_graph_menu_open);

    /* UI Tweaking */
    if (ImGui::Begin("Controls")) {
      ImGui::SliderFloat("rounding", &g_rounding, 0.0, 10);
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
