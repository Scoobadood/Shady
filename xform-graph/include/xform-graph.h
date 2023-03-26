/*
 * An XformGraph is a directed graph of transformations. Each node is an Xform
 * Edges represent the connection of an output from one Xform to the input of another.
 * Since Xforms may have multiple outputs and inputs there may be multiple edges from one Xform
 * node to another. For example:
 *
 *
 * +------------+       +------------+
 * |  Split   R +-------+ R   Merge  |
 * | Channels G +-------+ B Channels |
 * |          B +-------+ G          |
 * +------------+       +------------+
 */
#ifndef IMAGE_TOYS_XFORM_GRAPH_H
#define IMAGE_TOYS_XFORM_GRAPH_H

#include "xforms/xform.h"

#include <spdlog/spdlog-inl.h>

#include <string>
#include <map>
#include <set>
#include <deque>

enum XformState {
  UNCONFIGURED, /* One or more config items are missing */
  INVALID, /* A required input is missing */
  STALE, /* Inputs have changed or been reconfigured since last evaluation */
  GOOD /* Evaluation is up to date and consistent with inputs */
};

class XformGraph {
public:
  bool add_xform(const std::shared_ptr<Xform> &xform);

  bool evaluate();

  std::shared_ptr<void> output_from(const std::string &xform_name, const std::string &port_name) const;

  /* @return a vector of all xforms */
  std::vector<std::shared_ptr<const Xform>> xforms() const;

  /* @return The names xform */
  std::shared_ptr<Xform> xform(const std::string &name) const;

  /* Delete the xform */
  bool delete_xform(const std::string &name);

  std::set<std::string>
  dependencies_for(const std::shared_ptr<Xform> &xform) const;

  /*
   * Connections
   */
  std::vector<
          std::pair<
                  std::pair<std::string, std::string>,
                  std::pair<std::string, std::string>>>
  connections() const;

  std::shared_ptr<std::pair<std::string, std::string>>
  connection_from(const std::string &xform, const std::string &port) const;

  std::shared_ptr<std::pair<std::string, std::string>>
  connection_to(const std::string &xform, const std::string &port) const;

  bool input_is_connected(const std::string &xform, const std::string &port) const;

  bool output_is_connected(const std::string &xform, const std::string &port) const;

  bool add_connection(const std::string &from_xform_name,
                      const std::string &from_port,
                      const std::string &to_xform_name,
                      const std::string &to_port);

  bool remove_connection(const std::string &to_xform_name,
                         const std::string &to_port);

  XformState state_for(const std::string &xform_name) const;


private:
  void refresh_state(const std::shared_ptr<Xform> &xform);

  void update_dependency_order();

  std::deque<std::shared_ptr<Xform>> ordered_xforms_;

  std::map<std::pair<std::string, std::string>, std::shared_ptr<void>> outputs_;
  std::map<std::string, std::shared_ptr<Xform>> xforms_by_name_;
  std::map<std::pair<std::string, std::string>, std::pair<std::string, std::string>> connections_from_;
  std::map<std::pair<std::string, std::string>, std::pair<std::string, std::string>> connections_to_;
  std::map<std::string, XformState> states_;
  std::map<std::string, uintmax_t> evaluation_times_;
};

#endif //IMAGE_TOYS_XFORM_GRAPH_H
