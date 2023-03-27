#ifndef IMAGE_TOYS_UI_STATE_H
#define IMAGE_TOYS_UI_STATE_H

#include "xform-graph.h"
#include "imgui.h"

struct State {
  std::shared_ptr<XformGraph> graph;

  /* Only set if we are actively connecting */
  std::shared_ptr<const Xform> connecting_xform;
  std::shared_ptr<const InputPortDescriptor> connecting_input;
  std::shared_ptr<const OutputPortDescriptor> connecting_output;
  ImVec2 conn_position;

  /* Currently selected output port index for each xform */
  std::map<std::string, int> selected_output;

  inline bool is_connecting() const {
    return connecting_input || connecting_output;
  }
};

#endif //IMAGE_TOYS_UI_STATE_H
