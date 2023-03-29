#include "imgui.h"
#include "file_utils.h"
#include "ui_state.h"
#include "xform-io.h"

#include <vector>
#include <string>

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


void do_open_file(bool &open_file, State &state) {
  ImGui::OpenPopup("XXX");

  if (ImGui::BeginPopupModal("XXX")) {
    state.connecting_input = nullptr;
    state.connecting_output = nullptr;
    state.connecting_xform = nullptr;

    ImGui::Text("Open a graph");
    static int selected_item = 0;
    uint32_t num_graph_files;
    auto graph_file_names = get_graph_file_names(num_graph_files);
    ImGui::ListBox("##graphs", &selected_item, graph_file_names, num_graph_files);

    if (ImGui::Button("Cancel")) {
      open_file = false;
      ImGui::CloseCurrentPopup();
    }

    // Disable iOpen button if no files
    if (num_graph_files == 0)
      ImGui::BeginDisabled();

    if (ImGui::Button("Open")) {
      open_file = false;
      if (graph_file_names) {
        auto graph = load_graph(graph_file_names[selected_item]);
        if (graph) {
          state.graph = graph;
          state.graph_file_name = graph_file_names[selected_item];
          state.graph->evaluate();
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

void   do_close_file(bool & close_file, State & state) {
  state.graph_file_name = "";
  state.graph = nullptr;
  close_file = false;
}

void do_save_file(bool &save_file, State &state) {
  if( state.graph_file_name.empty())
    state.graph_file_name = "unknown.json";
  save_graph(state.graph_file_name, state.graph);
  save_file = false;
}

void do_menus(bool &open_graph_menu_open, State &state) {
  static bool open_file = false;
  static bool save_file = false;
  static bool close_file = false;

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Graph")) {
      ImGui::MenuItem("Open...", "Cmd+O", &open_file);
      ImGui::MenuItem("Save", "Cmd+S", &save_file);
      ImGui::MenuItem("Close", "Cmd+X", &close_file);
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (open_file) do_open_file(open_file, state);
  if (save_file) do_save_file(save_file, state);
  if (close_file) do_close_file(close_file, state);
}