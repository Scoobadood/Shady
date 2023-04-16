#include "imgui.h"
#include "ui_state.h"
#include "ui_graph.h"
#include "ui_theme.h"
#include "ui_xform_config.h"

#include "xform-texture-meta.h"
#include "imgui_internal.h"

struct XformRenderContext {
  std::shared_ptr<const Xform> xform;
  std::shared_ptr<const InputPortDescriptor> input_port;
  std::shared_ptr<const OutputPortDescriptor> output_port;
  ImVec2 port_pos;
};

bool is_mouse_over_port(const ImVec2 &mouse_pos,
                        const ImVec2 &port_pos,
                        float port_radius) {
  if (fabsf(mouse_pos.x - port_pos.x) > port_radius) return false;
  if (fabsf(mouse_pos.y - port_pos.y) > port_radius) return false;
  return true;
}

/* Render a port
 * Coloured according to whether it's an input or output
 * Filled according to whether its connected or not
 * Sized based on whether mouse is over
 * Handles mouse down and up
 */
void render_port_connector(int id, bool is_input,
                           bool is_connected,
                           bool connect_compatible,
                           XformRenderContext &context,
                           State &state) {

  auto line_height = ImGui::GetTextLineHeight();

  if (ImGui::BeginChild(id, {line_height, line_height}, false, ImGuiWindowFlags_NoMove)) {

    auto port_radius = 0.3f * line_height;
    auto p1 = ImGui::GetWindowPos();
    auto p2 = ImGui::GetWindowSize();
    context.port_pos = {p1.x + p2.x * 0.5f, p1.y + p2.y * 0.5f};

    // Determine colour, size and shape of port connector
    auto pos = ImGui::GetMousePos();
    bool over_port = is_mouse_over_port(pos, context.port_pos, port_radius);

    /*
     * 0 == small open circle
     * 1 == large open circle
     * 2 == small filled circle
     * 3 == large filled circle
     * 4 == cross
     */
    int shape;
    ImColor colour;
    if (over_port && state.is_connecting()) {
      if (connect_compatible) {
        colour = is_input ? g_conn_in_port_colour : g_conn_out_port_colour;
        shape = is_connected ? 3 : 1;
      } else {
        colour = 0xffffffff;
        shape = 4;
      }
    } else if (over_port) {
      colour = is_input ? g_conn_in_port_colour : g_conn_out_port_colour;
      shape = is_connected ? 3 : 1;
    } else {
      colour = is_input ? g_conn_in_port_colour : g_conn_out_port_colour;
      shape = is_connected ? 2 : 0;
    }


    auto dl = ImGui::GetWindowDrawList();
    switch (shape) {
      case 0:
        dl->AddCircle(context.port_pos, port_radius, colour);
        break;
      case 1:
        dl->AddCircle(context.port_pos, port_radius * 1.2f, colour);
        break;
      case 2:
        dl->AddCircleFilled(context.port_pos, port_radius, colour);
        break;
      case 3:
        dl->AddCircleFilled(context.port_pos, port_radius * 1.2f, colour);
        break;
      case 4:
        dl->AddLine({context.port_pos.x - port_radius, context.port_pos.y - port_radius},
                    {context.port_pos.x + port_radius, context.port_pos.y + port_radius},
                    colour);
        dl->AddLine({context.port_pos.x - port_radius, context.port_pos.y + port_radius},
                    {context.port_pos.x + port_radius, context.port_pos.y - port_radius},
                    colour);
        break;
    }

    /*
     * Check for mouse down in port
     */
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
      if (over_port) {
        state.connecting_xform = context.xform;
        state.conn_position = context.port_pos;
        if (is_input) {
          state.connecting_input = context.input_port;
        } else {
          state.connecting_output = context.output_port;
        }
      }
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
      if (over_port) {
        if (connect_compatible) {
          // Do the connection.
          if (is_input) {
            state.graph->add_connection(
                    state.connecting_xform->name(),
                    state.connecting_output->name(),
                    context.xform->name(),
                    context.input_port->name()
            );
            state.graph->evaluate();
          } else {
            state.graph->add_connection(
                    context.xform->name(),
                    context.output_port->name(),
                    state.connecting_xform->name(),
                    state.connecting_input->name()
            );
            state.graph->evaluate();
          }
        }
        state.connecting_xform = nullptr;
        state.connecting_input = nullptr;
        state.connecting_output = nullptr;
      }
    }

    //

    // Popup delete menu if port is ocnnected
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
      if (over_port && is_connected) {
        ImGui::OpenPopup("Frung", ImGuiPopupFlags_MouseButtonRight);
      }
    }
    if (ImGui::BeginPopup("Frung", 0) ){
      if (ImGui::Selectable("Delete")) {
        if( is_input) {
          // Delete connection from other output
          state.graph->remove_connection(context.xform->name(),
                                         context.input_port->name());

        } else {
          // Delete connection to output
          // TODO: Handle multiple connections
          // Delete connection from other output
          auto to = state.graph->connection_from(context.xform->name(), context.output_port->name());
          state.graph->remove_connection(to->first, to->second);
        }

      }
      ImGui::EndPopup();
    }


    //

  }
  ImGui::EndChild();
}

/*
 *
 */
bool render_port_name(int id, const std::string &name, bool is_selected) {
  bool selected = false;
  auto line_height = ImGui::GetTextLineHeight();

  if (ImGui::BeginChild(id, {-line_height, line_height})) {
    if (is_selected) {
      ImGui::PushStyleColor(ImGuiCol_Text, 0xffffffff);
    }
    ImGui::Text("%s", name.c_str());
    if (is_selected) {
      ImGui::PopStyleColor(1);
    }

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
      selected = true;
    }
  }
  ImGui::EndChild();
  return selected;
}

bool can_connect(const State &state, const std::shared_ptr<const InputPortDescriptor> &ipd) {
  if (!state.connecting_output) return false;
  return ipd->is_compatible(*state.connecting_output);
}

bool can_connect(const State &state, const std::shared_ptr<const OutputPortDescriptor> &opd) {
  if (!state.connecting_input) return false;
  return state.connecting_input->is_compatible(*opd);
}

/*
 * Render an input port
 */
void render_input_port(int ipd_idx,
                       bool is_connected,
                       XformRenderContext &context,
                       State &state
) {
  bool connect_compatible = can_connect(state, context.input_port);
  render_port_connector(ipd_idx + 1, true, is_connected,
                        connect_compatible, context, state);

  ImGui::SameLine(0, 0);

  render_port_name(ipd_idx + 100, context.input_port->name(), false);
}

/*
 * Render an output port
 */
bool render_output_port(int opd_idx,
                        bool is_selected,
                        bool is_connected,
                        XformRenderContext &context,
                        State &state
) {
  bool connect_compatible = can_connect(state, context.output_port);
  bool selected = render_port_name(opd_idx + 100, context.output_port->name(), is_selected);

  ImGui::SameLine(0, 0);

  render_port_connector(opd_idx + 1, false, is_connected,
                        connect_compatible,
                        context,
                        state);
  return selected;
}


/*
 * Render input ports for a specific Xform
 */
void render_input_ports(const std::shared_ptr<const Xform> &xform,
                        const std::vector<bool> &is_connected,
                        std::map<std::pair<std::string, std::string>, ImVec2> &in_port_coords,
                        State &state) {


  auto port_descriptors = xform->input_port_descriptors();
  auto num_inputs = port_descriptors.size();

  XformRenderContext context{xform, nullptr, nullptr};

  ImGui::PushStyleColor(ImGuiCol_ChildBg, g_in_port_bg_colour);
  if (ImGui::BeginChild("##inputs", {ImGui::GetWindowWidth() * 0.5f, 0})) {

    for (int ipd_idx = 0; ipd_idx < num_inputs; ipd_idx++) {
      context.input_port = port_descriptors.at(ipd_idx);

      const bool is_port_connected = is_connected.at(ipd_idx);

      render_input_port(ipd_idx, is_port_connected, context, state);
      in_port_coords.emplace(std::make_pair(context.xform->name(),
                                            context.input_port->name()),
                             context.port_pos);
    }
  }
  ImGui::EndChild();
  ImGui::PopStyleColor(1);
}

/*
 * Render input ports for a specific Xform
 */
void render_output_ports(const std::shared_ptr<const Xform> &xform,
                         const std::vector<bool> &is_connected,
                         std::map<std::pair<std::string, std::string>, ImVec2> &out_port_coords,
                         State &state) {

  auto port_descriptors = xform->output_port_descriptors();
  auto num_outputs = port_descriptors.size();

  ImGui::PushStyleColor(ImGuiCol_ChildBg, g_out_port_bg_colour);
  ImGui::PushStyleColor(ImGuiCol_Border, g_xform_bg_colour);

  XformRenderContext context{xform, nullptr, nullptr};

  int &current_selection = state.selected_output[xform->name()];

  if (ImGui::BeginChild("##outputs", {ImGui::GetWindowWidth() * 0.5f, 0})) {

    for (int opd_idx = 0; opd_idx < num_outputs; opd_idx++) {
      context.output_port = port_descriptors.at(opd_idx);

      const bool is_selected = (current_selection == opd_idx);
      const bool is_port_connected = is_connected.at(opd_idx);

      if (render_output_port(opd_idx, is_selected, is_port_connected, context, state)) {
        current_selection = opd_idx;
      }
      out_port_coords.emplace(std::make_pair(xform->name(), context.output_port->name()), context.port_pos);
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
                  std::map<std::pair<std::string, std::string>, ImVec2> &out_port_coords,
                  State &state
) {
  auto num_input_ports = xform->input_port_descriptors().size();
  auto num_output_ports = xform->output_port_descriptors().size();
  auto max_ports = std::max(num_input_ports, num_output_ports);

  // Build UI details
  auto height = ImGui::GetTextLineHeightWithSpacing() * (float) max_ports + (ImGui::GetStyle().ChildBorderSize * 2);

  ImGui::BeginChild("##ports", {0, height}, true);

  std::vector<bool> inputs_connected;
  inputs_connected.reserve(num_input_ports);
  for (auto ip_idx = 0; ip_idx < num_input_ports; ++ip_idx) {
    auto ipd = xform->input_port_descriptors().at(ip_idx);
    inputs_connected.push_back(graph->input_is_connected(xform->name(), ipd->name()));
  }
  render_input_ports(xform, inputs_connected, in_port_coords, state);

  ImGui::SameLine(0, 0);

  std::vector<bool> outputs_connected;
  outputs_connected.reserve(num_output_ports);
  for (auto op_idx = 0; op_idx < num_output_ports; ++op_idx) {
    auto opd = xform->output_port_descriptors().at(op_idx);
    outputs_connected.push_back(graph->output_is_connected(xform->name(), opd->name()));
  }

  render_output_ports(xform, outputs_connected, out_port_coords, state);
  ImGui::EndChild();

}

/*
 * If an Xform has one or more outputs, enable a collapsing section
 * to render the selected output.
 */
void maybe_render_output_vignette(const std::shared_ptr<const Xform> &xform,
                                  State &state) {
  if (xform->output_port_descriptors().empty()) return;

  if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_None)) {
    auto selected_output_idx = state.selected_output[xform->name()];
    auto selected_opd = xform->output_port_descriptors().at(selected_output_idx);
    auto output = state.graph->output_from(xform->name(), selected_opd->name());
    if (output) {
      auto tx = (TextureMetadata *) output.get();
      auto aspect = (float) tx->height / (float) tx->width;
      auto scaled_height = ImGui::GetWindowSize().x * aspect;
      ImGui::Image((ImTextureID) (tx->texture_id), ImVec2(ImGui::GetWindowSize().x, scaled_height));
    }
  }
}

ImVec4 border_colour_for_state(XformState s) {
  switch (s) {
    case UNCONFIGURED:
      return g_unconfigured_xf_border;
    case INVALID:
      return g_invalid_xf_border;
    case STALE:
      return g_stale_xf_border;
    case GOOD:
      return g_good_xf_border;
    default:
      return ImVec4(255, 0, 255, 255);
  }
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
                  std::map<std::pair<std::string, std::string>, ImVec2> &out_port_coords,
                  State &state,
                  const std::shared_ptr<Theme> &theme
) {
  // TODO: Work out dynamic sizing later
  ImGui::SetNextWindowSize(ImVec2(190.0f, 0));

  /* Own window per xform */
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
  auto s = state.graph->state_for(xform->name());
  ImGui::PushStyleColor(ImGuiCol_Border, border_colour_for_state(s));

  auto title = fmt::format("{}:{}", xform->name(), xform->type());
  ImGui::Begin(title.c_str(), nullptr,
               ImGuiWindowFlags_NoResize
               | ImGuiWindowFlags_NoCollapse
  );


  auto l = ImGui::GetTextLineHeightWithSpacing();
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0, 0});
  if (ImGui::BeginChild("##toolbar", {0, l})) {
    auto w = ImGui::GetWindowWidth() - 2 * l;
    ImGui::Indent(w);
    if (ImGui::ImageButton("##1", (ImTextureID) theme->btn_tx_,
                           {l, l}, {0., 0},
                           {64. / 512., 64. / 256.})) {
      graph->evaluate();
    }
    ImGui::SameLine(0, 0);
    if (ImGui::ImageButton("##2", (ImTextureID) theme->btn_tx_,
                           {l, l}, {64. / 512., 0},
                           {128. / 512., 64. / 256.})) {
      graph->delete_xform(xform->name());
    }
    ImGui::Unindent(w);
  }
  ImGui::EndChild();
  ImGui::PopStyleVar(1);

  render_ports(graph, xform, in_port_coords, out_port_coords, state);

  maybe_render_output_vignette(xform, state);

  maybe_render_config(xform, state);

  ImGui::End();
  ImGui::PopStyleColor(1);
  ImGui::PopStyleVar(1);
}

/*
 * Render the graph by rendering the nodes.
 */
void render_graph(State &state, const std::shared_ptr<Theme> &theme) {
  using namespace std;

  /*
   * Each node is a window that controls its own port locations.
   * These maps get populated as nodes are drawn and then connections
   * are drawn separately.
   */
  map<pair<string, string>, ImVec2> in_port_coords;
  map<pair<string, string>, ImVec2> out_port_coords;

  /* For each xform in the graph, make a node
   */
  for (const auto &xform: state.graph->xforms()) {
    render_xform(state.graph, xform, in_port_coords, out_port_coords, state, theme);
  }

  /*
   * Render connections between the nodes
   */
  for (const auto &conn: state.graph->connections()) {
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

  /* Handle rubber banding of new connections */
  if (state.is_connecting()) {//} ImGui::IsMouseDragging(ImGuiMouseButton_Left, 2)) {
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
