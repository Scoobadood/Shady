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
#ifndef SHADY_XFORM_GRAPH_H
#define SHADY_XFORM_GRAPH_H

#include "xforms/xform.h"

#include <spdlog/spdlog-inl.h>

#include <string>
#include <map>
#include <set>
#include <deque>
#include <utility>

enum XformState {
  UNCONFIGURED, /* One or more config items are missing */
  INVALID, /* A required input is missing */
  ERROR, /* Evaluate failed for some reason despite bveing correctly configured and wired */
  STALE, /* Inputs have changed or been reconfigured since last evaluation */
  GOOD /* Evaluation is up to date and consistent with inputs */
};

struct Port {
  std::string xform_name;
  std::string port_name;
protected:
  Port(std::string xform_name, std::string port_name)//
          : xform_name{std::move(xform_name)} //
          , port_name{std::move(port_name)}//
  {}
};

struct XformInputPort : public Port {
  XformInputPort(const std::string &xform_name, const std::string &port_name)//
          : Port{xform_name, port_name}//
  {}
  friend bool operator<(const XformInputPort &lhs, const XformInputPort &rhs) {
    if (lhs.xform_name < rhs.xform_name) return true;
    if (lhs.xform_name > rhs.xform_name) return false;
    return lhs.port_name < rhs.port_name;
  }
};


struct XformOutputPort : public Port {
  XformOutputPort(const std::string &xform_name, const std::string &port_name)//
          : Port{xform_name, port_name}//
  {}

  friend bool operator<(const XformOutputPort &lhs, const XformOutputPort &rhs) {
    if (lhs.xform_name < rhs.xform_name) return true;
    if (lhs.xform_name > rhs.xform_name) return false;
    return lhs.port_name < rhs.port_name;
  }
};

class XformGraph {
public:
  /**
   * Add the new xform to the graph.
   * @param xform The xform to add.
   * @throws XformExists if it does.
   */
  void add_xform(const std::shared_ptr<Xform> &xform);

  bool evaluate();

  /**
   * @return the computed result for a particular transform and port name.
   */
  std::shared_ptr<void> result_at(const XformOutputPort &port) const;

  /* @return a vector of all xforms */
  std::vector<std::shared_ptr<const Xform>> xforms() const;

  /* @return The named xform */
  std::shared_ptr<Xform> xform(const std::string &name) const;

  /* delete the xform */
  void delete_xform(const std::string &name);

  std::set<std::string>
  dependencies_for(const std::shared_ptr<Xform> &xform) const;

  /*
   * Connections
   */
  std::vector<std::pair<XformOutputPort, XformInputPort>>
  connections() const;

  /*
   * @return a vector of pairs of <xform,port> for all
   * ports connected from the specified output.
   */
  std::shared_ptr<XformInputPort>
  connection_from(const XformOutputPort &port) const;

  /*
   * @return a pointer to the XformOutputPort which is connected to
   * the specified input or nullptr if none.
   */
  std::shared_ptr<XformOutputPort>
  connection_to(const XformInputPort &input_port) const;

  /*
   * @return True if the given input is connected to any output. Otherwise False.
   * NB if the xform or port is invalid then this will return false and log an error message.
   */
  bool is_connected(const XformInputPort &port) const;

  /*
   * @return True if the given input is connected to any output. Otherwise False.
   * NB if the xform or port is invalid then this will return false and log an error message.
   */
  bool is_connected(const XformOutputPort &port) const;

  /**
   * Add a connection from the specified xform and output port to
   * another xform input port.
   * If the connection already exists a warning is logged but the connection remains unchanged.
   * If another connection is in place to the target input port then it is disconnected and a warning is logged.
   * If the connection cannot be made (one or other xform or port doesn't exist) then an error is logged and
   * the method returns false.
   *
   * @param output   The transform and output port from which to connect.
   * @param input    The transform and input port to which to connect.
   * @param to_port         The input port.
   */
  void connect(const XformOutputPort &output,
               const XformInputPort &input);

  /**
   * Remove the connection from the output to the input.
   * If the connection does not exists or one of the ports is incorrect, will throw
   *
   * @param output The src transform and port from disconnect.
   * @param input  The tgt transform and port from which to disconnect.
   */
  void
  disconnect(const XformOutputPort &output,
             const XformInputPort &input);

  /**
   * Remove the connection to the specified input. Since an input can only
   * have a single connection, this is unambiguous.
   * If the connection does not exist or the port is incorrect, will throw
   *
   * @param input         The input port.
   */
  void
  disconnect(const XformInputPort &input);

  XformState state_for(const std::string &xform_name) const;

  /**
   * @return a name of the form <xform_type_name>_NNN where the name is not used in the graph and
   * NNN may be an arbitrary number of digits but will be only sufficient to guarantee a name's uniqueness.
   * If there is no xform called 'xform_type_name' then this will be te returned value.
   * Otherwise it will return xform_type_name_1, xform_type_name_2, xform_type_name_3 etc.
   */
  std::string next_free_name_like(const std::string & base_name);

private:
  void validate_xform(const std::string &xform_name) const;

  std::shared_ptr<const OutputPortDescriptor>
  output_pd_or_throw(const XformOutputPort &port) const;

  std::shared_ptr<const InputPortDescriptor>
  input_pd_or_throw(const XformInputPort &port) const;

  void refresh_state(const std::shared_ptr<Xform> &xform);

  void update_dependency_order();

  std::deque<std::shared_ptr<Xform>> ordered_xforms_;

  std::map<XformOutputPort, std::shared_ptr<void>> results_;
  std::map<std::string, std::shared_ptr<Xform>> xforms_by_name_;

  std::map<XformOutputPort, XformInputPort> connections_from_;
  std::map<XformInputPort, XformOutputPort> connections_to_;
  std::map<std::string, XformState> states_;
  std::map<std::string, uintmax_t> evaluation_times_;
};

#endif //SHADY_XFORM_GRAPH_H
