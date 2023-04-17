#include "xform-graph.h"
#include "xform-exceptions.h"
#include "xforms/xform.h"

#include <set>
#include <map>
#include <deque>
#include <regex>

#include <spdlog/spdlog-inl.h>

void XformGraph::validate_xform(const std::string &xform_name) const {
  auto it = xforms_by_name_.find(xform_name);
  if (it != xforms_by_name_.end())
    return;

  throw XformGraphException(fmt::format("No such xform : {}", xform_name),
                            ERR_NO_SUCH_XFORM);
}

/*
 * Add a new transform to the graph. If it's successfully added
 * initialise its state.
 */
void
XformGraph::add_xform(const std::shared_ptr<Xform> &xform) {
  if (!xform)
    throw XformGraphException(fmt::format("Xform is null in add_xform()."));


  if (xforms_by_name_.find(xform->name()) != xforms_by_name_.end()) {
    throw XformGraphException(
            fmt::format("Xform {} already in graph in add_xform().", xform->name()),
            ERR_XFORM_EXISTS);
  }

  xforms_by_name_.emplace(xform->name(), xform);
  evaluation_times_[xform->name()] = 0;
  refresh_state(xform);
}


/**
 * @return the computed result for a particular transform and port name.
 */
std::shared_ptr<void> XformGraph::result_at(const XformOutputPort &port) const {
  output_pd_or_throw(port);

  auto it = results_.find(port);
  if (it != results_.end())
    return it->second;

  spdlog::warn("No result for {}::{}", port.xform_name, port.port_name);
  return nullptr;
}

/*
 * Delete the xform
 * and all associated connections to and from others
 * and associated metadata.
 * Refresh the state of downstream xforms.
 */
void XformGraph::delete_xform(const std::string &name) {
  validate_xform(name);

  // Delete the connections from this xform to others (remember their names)
  std::set<std::string> impacted_xforms;
  for (auto from_it = connections_from_.begin(); from_it != connections_from_.end();) {
    if (from_it->first.first == name) {
      impacted_xforms.emplace(from_it->second.first);

      // Only one connection is allowed into a port so the
      // connection_to_ entry for this from can be deleted
      connections_to_.erase(from_it->second);

      from_it = connections_from_.erase(from_it);
    } else {
      ++from_it;
    }
  }


  // Delete the connections to this xform
  for (auto to_it = connections_to_.begin(); to_it != connections_to_.end();) {
    if (to_it->first.first == name) {
      // Currently only one connection is allowed from a port to another
      // So erase the connection_from_ to this.
      connections_from_.erase(to_it->second);

      to_it = connections_to_.erase(to_it);
    } else {
      ++to_it;
    }
  }
  // Delete evaluation times and states
  states_.erase(name);
  evaluation_times_.erase(name);

  // Delete cached outputs
  for (auto op_iter = results_.begin(); op_iter != results_.end();) {
    if (op_iter->first.xform_name != name) {
      ++op_iter;
    } else {
      op_iter = results_.erase(op_iter);
    }
  }

  // And finally remove the xform.
  xforms_by_name_.erase(name);

  // Recompute the dependency sequences
  update_dependency_order();

  // Now refresh the state of the xforms in order
  for (const auto &xf: ordered_xforms_) {
    refresh_state(xf);
  }
}


std::shared_ptr<Xform> XformGraph::xform(const std::string &name) const {
  validate_xform(name);
  return  xforms_by_name_.at(name);
}

std::shared_ptr<const OutputPortDescriptor>
XformGraph::output_pd_or_throw(const XformOutputPort &port) const {
  validate_xform(port.xform_name);
  auto xform = xforms_by_name_.at(port.xform_name);
  auto pd = xform->output_port_descriptor_for_port(port.port_name);
  if (pd) return pd;

  throw XformGraphException(fmt::format("No output port : {} on xform {}", port.xform_name, port.port_name),
                            ERR_NO_SUCH_OUTPUT_PORT);
}


/**
 * @return the input port descriptor for the given transform and name or else throw an exception.
 */
std::shared_ptr<const InputPortDescriptor>
XformGraph::input_pd_or_throw(const XformInputPort &port) const {
  validate_xform(port.xform_name);
  auto xform = xforms_by_name_.at(port.xform_name);
  auto pd = xform->input_port_descriptor_for_port(port.port_name);
  if (pd) return pd;

  throw XformGraphException(fmt::format("No input port : {} on xform {}", port.xform_name, port.port_name),
                            ERR_NO_SUCH_INPUT_PORT);
}


void XformGraph::connect(const XformOutputPort &from,
                         const XformInputPort &to) {

  auto from_pd = output_pd_or_throw(from);
  auto to_pd = input_pd_or_throw(to);

  if (!to_pd->is_compatible(*from_pd)) {
    throw XformGraphException(fmt::format("Ports {}::{} and {}::{} are incompatible",
                                          from.xform_name, from.port_name,
                                          to.xform_name, to.port_name),
                              ERR_PORTS_ARE_INCOMPATIBLE);
  }

  // TODO: Check for introduced loops

  // Remove any existing from connection
  auto conn_it = connections_from_.find({from.xform_name, from.port_name});
  if (conn_it != connections_from_.end()) {
    auto old_to_xform = conn_it->second.first;
    auto old_to_port = conn_it->second.second;
    spdlog::info("Removing existing connection from {}::{} to {}::{}",
                 from.xform_name, from.port_name,
                 old_to_xform, old_to_port);
    connections_from_.erase(conn_it);
    connections_to_.erase({old_to_xform, old_to_port});
  }

  // And any existing to connection
  conn_it = connections_to_.find({to.xform_name, to.port_name});
  if (conn_it != connections_to_.end()) {
    auto old_from_xform = conn_it->second.first;
    auto old_from_port = conn_it->second.second;
    spdlog::info("Removing existing connection to {}::{} from {}::{}",
                 to.xform_name, to.port_name,
                 old_from_xform, old_from_port);

    connections_to_.erase(conn_it);
    connections_from_.erase({old_from_xform, old_from_port});
  }

  connections_from_[{from.xform_name, from.port_name}] = {to.xform_name, to.port_name};
  connections_to_[{to.xform_name, to.port_name}] = {from.xform_name, from.port_name};

  update_dependency_order();
  for (const auto &xf: ordered_xforms_) {
    refresh_state(xf);
  }
}

void
XformGraph::disconnect(const XformInputPort &input) {
  input_pd_or_throw(input);

  auto it = connections_to_.find({input.xform_name, input.port_name});
  if (it == connections_to_.end()) {
    spdlog::warn(fmt::format("Port {}::{} is not connected in disconnect()", input.xform_name, input.port_name));
    return;
  }


  connections_from_.erase(it->second);
  connections_to_.erase(it);

  update_dependency_order();
  for (const auto &xf: ordered_xforms_) {
    refresh_state(xf);
  }
}

void
XformGraph::disconnect(const XformOutputPort &output,
                       const XformInputPort &input) {
  input_pd_or_throw(input);

  auto it = connections_to_.find({input.xform_name, input.port_name});
  if (it == connections_to_.end()) {
    spdlog::warn(fmt::format("Port {}::{} is not connected in disconnect()", input.xform_name, input.port_name));
    return;
  }

  // Validate that the output is correct
  if (it->second.first != output.xform_name || it->second.second != output.port_name) {
    spdlog::warn(fmt::format("Port {}::{} not connected to {}::{} in disconnect()",
                             output.xform_name, output.port_name,
                             input.xform_name, input.port_name));
    return;
  }

  connections_from_.erase(it->second);
  connections_to_.erase(it);

  update_dependency_order();
  for (const auto &xf: ordered_xforms_) {
    refresh_state(xf);
  }
}

std::set<std::string>
XformGraph::dependencies_for(const std::shared_ptr<Xform> &xform) const {
  using namespace std;

  set<string> deps;

  for (const auto &ipd: xform->input_port_descriptors()) {
    auto iter = connections_to_.find({xform->name(), ipd->name()});
    if (iter == connections_to_.end()) {
      assert(!ipd->is_required());
      continue;
    }
    deps.emplace(iter->second.first);
  }
  return deps;
}

/*
 * Find all sinks and backwards chain until they are satisfied.
 * If we evaluate all xforms and the sinks are still unsatisfied
 * return false otherwise return true.
 * TODO: We want to be lazy about this and not force re-evaluate things that are still
 *       valid. We need to decide if we have xforms cache results or do it in the graph.
 *       For now we will re-evaluate everything.
 */
bool XformGraph::evaluate() {
  using namespace std;

  set<string> failed_deps;
  results_.clear();
  for (const auto &xf: ordered_xforms_) {
    auto state = state_for(xf->name());
    if (state != GOOD && state != STALE) continue;

    // Obtain the set of dependencies.
    auto deps = dependencies_for(xf);

    // If any dep is failed, then curr_xform will fail
    if (any_of(deps.begin(), deps.end(), [&failed_deps](const string &dep) {
      return failed_deps.count(dep) != 0;
    })) {
      failed_deps.emplace(xf->name());
      continue;
    }

    // All deps are resolved. Resolve this xform.
    map<string, shared_ptr<void>> inputs;
    for (const auto &ipd: xf->input_port_descriptors()) {
      auto iter = connections_to_.find({xf->name(), ipd->name()});
      if (iter == connections_to_.end()) continue;
      inputs.emplace(ipd->name(), results_.at({iter->second.first, iter->second.second}));
    }

    uint32_t err;
    std::string err_msg;
    auto out = xf->apply(inputs, err, err_msg);
    if (err == XFORM_OK) {
      for (const auto &o: out) {
        results_.emplace(XformOutputPort{xf->name(), o.first}, o.second);
      }
      states_[xf->name()] = GOOD;
      evaluation_times_[xf->name()] = ::clock();
    } else {
      states_[xf->name()] = ERROR;
      spdlog::error("Xform {} failed with code {}: {}", xf->name(), err, err_msg);
      failed_deps.emplace(xf->name());
    }
  }
  return true;
}

std::vector<std::shared_ptr<const Xform>>
XformGraph::xforms() const {
  using namespace std;
  vector<shared_ptr<const Xform>> xforms;
  xforms.reserve(xforms_by_name_.size());
  for (auto &e: xforms_by_name_) {
    xforms.push_back(e.second);
  }
  return xforms;
}

std::vector<std::pair<XformOutputPort, XformInputPort>>
XformGraph::connections() const {
  using namespace std;
  vector<pair<XformOutputPort, XformInputPort>> conns;
  conns.reserve(connections_from_.size());
  for (auto &fcon: connections_from_) {
    conns.emplace_back(XformOutputPort{fcon.first.first, fcon.first.second},
                       XformInputPort{fcon.second.first, fcon.second.second});
  }
  return conns;
}

std::shared_ptr<XformInputPort>
XformGraph::connection_from(const XformOutputPort &port) const {
  auto it = connections_from_.find({port.xform_name, port.port_name});
  if (it == connections_from_.end()) return nullptr;
  return std::make_shared<XformInputPort>(it->second.first, it->second.second);
}

std::shared_ptr<XformOutputPort>
XformGraph::connection_to(const XformInputPort &input_port) const {
  auto it = connections_to_.find({input_port.xform_name, input_port.port_name});
  if (it == connections_to_.end()) return nullptr;
  return std::make_shared<XformOutputPort>(it->second.first, it->second.second);
}

/*
 * @return true if the given input is connected
 */
bool XformGraph::is_connected(const XformInputPort &port) const {
  input_pd_or_throw(port);
  auto iter = connections_to_.find({port.xform_name, port.port_name});
  return !(iter == connections_to_.end());
}

/*
 * @return false if the given output is connected
 */
bool XformGraph::is_connected(const XformOutputPort &port) const {
  output_pd_or_throw(port);
  auto iter = connections_from_.find({port.xform_name, port.port_name});
  return !(iter == connections_from_.end());
}

/*
 * We cache states and assume that each xform has a state.
 */
XformState XformGraph::state_for(const std::string &xform_name) const {
  validate_xform(xform_name);
  return states_.at(xform_name);
}

/*
 * We assume that the xform has just been added and update its state accordingly.
 *
 */
void XformGraph::refresh_state(const std::shared_ptr<Xform> &xform) {
  using namespace std;

  // Check Config
  if (!xform->is_config_valid()) {
    states_[xform->name()] = UNCONFIGURED;
    return;
  }

  // Check [required] input connections
  vector<string> dependent_xforms;
  for (const auto &ipd: xform->input_port_descriptors()) {
    if (!ipd->is_required()) continue;
    auto it = connections_to_.find({xform->name(), ipd->name()});
    if (it == connections_to_.end()) {
      states_[xform->name()] = INVALID;
      return;
    }
    dependent_xforms.push_back(it->second.first);
  }

  // Check input validity and freshness
  for (const auto &xfn: dependent_xforms) {
    auto input_state = states_.at(xfn);
    if (input_state == UNCONFIGURED || input_state == INVALID) {
      states_[xform->name()] = INVALID;
      return;
    }
    /* The >= here is to manage the fact that newly added xforms have evaluation_times_
     * set to 0. Any node that has been evaluated already would make this stale but it's possible
     * that two ndes added together may see themselves fresher than each other unless we compare
     * for equality.
     */
    if (input_state == STALE || (evaluation_times_.at(xfn) >= evaluation_times_.at(xform->name()))) {
      states_[xform->name()] = STALE;
      return;
    }
  }
  states_[xform->name()] = GOOD;
}

/*
 * Use Kahn's algorithm to generate the new topological sort order
 * and then refresh the state of each node in order.
 * https://en.wikipedia.org/wiki/Topological_sorting
 */
void XformGraph::update_dependency_order() {
  using namespace std;

  ordered_xforms_.clear();

  set<string> no_marks;
  set<string> temp_marks;
  set<string> perm_marks;
  for (const auto &x: xforms_by_name_) {
    no_marks.emplace(x.second->name());
  }

  std::function<void(const std::string &)> visit = [&](const std::string &n) -> void {
    if (perm_marks.find(n) != perm_marks.end()) return;
    if (temp_marks.find(n) != temp_marks.end()) {
      throw std::runtime_error("Cycle in graph");
    }

    // Mark temporary mark
    no_marks.erase(n);
    temp_marks.emplace(n);

    // nodes_to_visit... downstream of n
    for (const auto &opd: xforms_by_name_.at(n)->output_port_descriptors()) {
      auto c = connection_from({n, opd->name()});
      if (c) {
        visit(c->xform_name);
      }
    }

    temp_marks.erase(n);
    perm_marks.emplace(n);
    ordered_xforms_.push_front(xforms_by_name_[n]);
  };

  while (!no_marks.empty()) {
    auto it = no_marks.begin();
    const auto n = *it;
    no_marks.erase(it);
    visit(n);
  }
}

/**
 * @return a name of the form <xform_type_name>_NNN where the name is not used in the graph and
 * NNN may be an arbitrary number of digits but will be only sufficient to guarantee a name's uniqueness.
 * If there is no xform called 'xform_type_name' then this will be te returned value.
 * Otherwise it will return xform_type_name_1, xform_type_name_2, xform_type_name_3 etc.
 */
std::string XformGraph::next_free_name_like(const std::string & base_name) {
  // Simple case, we can use the base name directly.
  if (xforms_by_name_.find(base_name) == xforms_by_name_.end()) return base_name;

  // Shortlist all names of the form base_name_NNN where NNN is an arbitrary number of digits.
  int32_t highest_index = 0;

  auto pattern_string = fmt::format("{}_([0-9]+)", base_name);
  std::regex pattern{pattern_string};

  for (const auto &xform_name_pair: xforms_by_name_) {
    auto xform_name = xform_name_pair.first;
    std::smatch matches;
    if (!regex_search(xform_name, matches, pattern)) continue;

    // 0 is the whole string, 1 is the frame
    int32_t idx = stoi(matches[1].str());
    if (idx > highest_index) highest_index = idx;
  }

  return fmt::format("{}_{}", base_name, highest_index + 1);
}
