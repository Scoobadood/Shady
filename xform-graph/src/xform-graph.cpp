#include "xform-graph.h"
#include "xforms/xform.h"

#include <set>
#include <map>
#include <deque>

#include <spdlog/spdlog-inl.h>

std::shared_ptr<void> XformGraph::output_from(const std::string &xform_name, const std::string &port_name) const {
  auto it = outputs_.find({xform_name, port_name});
  if (it == outputs_.end()) return nullptr;
  return it->second;
}

/*
 * Add a new transform to the graph. If it's successfully added
 * initialise its state.
 */
bool
XformGraph::add_xform(const std::shared_ptr<Xform> &xform) {
  if (xforms_by_name_.find(xform->name()) != xforms_by_name_.end()) {
    spdlog::error("Xform {} already in graph.", xform->name());
    return false;
  }

  xforms_by_name_.emplace(xform->name(), xform);
  evaluation_times_[xform->name()] = 0;
  refresh_state(xform);
  return true;
}

/*
 * Delete the xform
 * and all associated connections to and from others
 * and associated metadata.
 * Refresh the state of downstream xforms.
 */
bool XformGraph::delete_xform(const std::string &name) {
  auto it = xforms_by_name_.find(name);
  if (it == xforms_by_name_.end()) return false;

  // Delete the connections from this xform to others (remember their names)
  std::set<std::string> impacted_xforms;
  for (auto from_it = connections_from_.begin(); from_it != connections_from_.end();) {
    if (from_it->first.first == name) {
      impacted_xforms.emplace(from_it->second.first);
      from_it = connections_from_.erase(from_it);
    } else {
      ++from_it;
    }
  }

  // Delete the connections to this xform
  for (auto to_it = connections_to_.begin(); to_it != connections_to_.end();) {
    if (to_it->first.first == name) {
      to_it = connections_to_.erase(to_it);
    } else {
      ++to_it;
    }
  }
  // Delete evaluation times and states
  states_.erase(name);
  evaluation_times_.erase(name);

  // Delete cached outputs
  for(auto op_iter = outputs_.begin(); op_iter != outputs_.end(); ) {
    if(op_iter->first.first != name ) {
      ++op_iter;
    } else {
      op_iter = outputs_.erase(op_iter);
    }
  }

  // And finally remove the xform.
  xforms_by_name_.erase(it);

  // Recompute the dependency sequences
  update_dependency_order();

  // Now refresh the state of the xforms in order
  for (const auto &xf: ordered_xforms_) {
    refresh_state(xf);
  }

  return true;
}


std::shared_ptr<Xform> XformGraph::xform(const std::string &name) const {
  auto it = xforms_by_name_.find(name);
  if (it == xforms_by_name_.end()) return nullptr;
  return it->second;
}

bool XformGraph::add_connection(const std::string &from_xform_name,
                                const std::string &from_port_name,
                                const std::string &to_xform_name,
                                const std::string &to_port_name) {
  auto it = xforms_by_name_.find(from_xform_name);
  if (it == xforms_by_name_.end()) {
    spdlog::error("No such xform: {}", from_xform_name);
    return false;
  }
  auto from_xform = it->second;

  it = xforms_by_name_.find(to_xform_name);
  if (it == xforms_by_name_.end()) {
    spdlog::error("No such xform: {}", from_xform_name);
    return false;
  }
  auto to_xform = it->second;

  auto from_pd = from_xform->output_port_descriptor_for_port(from_port_name);
  if (from_pd == nullptr) {
    spdlog::error("No port : {} on xform {}", from_port_name, from_xform_name);
    return false;
  }

  auto to_pd = to_xform->input_port_descriptor_for_port(to_port_name);
  if (to_pd == nullptr) {
    spdlog::error("No port : {} on xform {}", to_port_name, to_xform_name);
    return false;
  }

  if (!to_pd->is_compatible(*from_pd)) {
    spdlog::error("Ports {}::{} and {}::{} are incompatible",
                  from_xform_name, from_port_name,
                  to_xform_name, to_port_name);
    return false;
  }

  // TODO: Check for introduced loops

  // Remove any existing from connection
  auto conn_it = connections_from_.find({from_xform_name, from_port_name});
  if (conn_it != connections_from_.end()) {
    auto old_to_xform = conn_it->second.first;
    auto old_to_port = conn_it->second.second;
    spdlog::info("Removing existing connection from {}::{} to {}::{}",
                 from_xform_name, from_port_name,
                 old_to_xform, old_to_port);
    connections_from_.erase(conn_it);
    connections_to_.erase({old_to_xform, old_to_port});
  }

  // And any existing to connection
  conn_it = connections_to_.find({to_xform_name, to_port_name});
  if (conn_it != connections_to_.end()) {
    auto old_from_xform = conn_it->second.first;
    auto old_from_port = conn_it->second.second;
    spdlog::info("Removing existing connection to {}::{} from {}::{}",
                 to_xform_name, to_port_name,
                 old_from_xform, old_from_port);

    connections_to_.erase(conn_it);
    connections_from_.erase({old_from_xform, old_from_port});
  }

  connections_from_[{from_xform_name, from_port_name}] = {to_xform_name, to_port_name};
  connections_to_[{to_xform_name, to_port_name}] = {from_xform_name, from_port_name};

  update_dependency_order();
  for (const auto &xf: ordered_xforms_) {
    refresh_state(xf);
  }

  return true;
}

bool XformGraph::remove_connection(const std::string &to_xform_name,
                                   const std::string &to_port) {
  auto it = connections_to_.find({to_xform_name, to_port});
  if (it == connections_to_.end()) {
    spdlog::error("No such xform: {}", to_xform_name);
    return false;
  }
  connections_to_.erase(it);

  update_dependency_order();
  for (const auto &xf: ordered_xforms_) {
    refresh_state(xf);
  }

  return true;
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

  /*
   * Some xforms may not have mandatory inputs connected.
   * If so, they cannot be evaluated. Flag them as failed first.
   */
  set<string> failed_xforms;
  for (const auto &xform: xforms_by_name_) {
    for (const auto &ipd: xform.second->input_port_descriptors()) {
      if (!ipd->is_required()) continue;
      if (connections_to_.find({xform.second->name(), ipd->name()}) != connections_to_.end()) continue;
      failed_xforms.emplace(xform.second->name());
    }
  }
  if (!failed_xforms.empty()) {
    spdlog::error("One or more xforms do not have required inputs connected");
  }

  /*
   * Add the sinks to a queue of xforms to be resolved
   */
  deque<shared_ptr<Xform>> to_satisfy;
  for (const auto &xform: xforms_by_name_) {
    if (xform.second->is_sink()) {
      to_satisfy.push_back(xform.second);
    }
  }

  // Evaluate until we're done or cannot continue.
  set<string> resolved_xforms;
  outputs_.clear();
  while (!to_satisfy.empty()) {
    const auto curr_xform = to_satisfy.back();

    // Obtain the set of dependencies. A dependency is a connection that must be satisfied
    // for this xform to be resolved.
    auto deps = dependencies_for(curr_xform);

    // If any dep is failed, then curr_xform will fail
    if (any_of(deps.begin(), deps.end(), [&failed_xforms](const string &dep) {
      return failed_xforms.count(dep) != 0;
    })) {
      failed_xforms.emplace(curr_xform->name());
      to_satisfy.pop_back();
      continue;
    }

    // If any dep is not yet considered, enqueue that transform
    bool has_unresolved = false;
    for (const auto &dep: deps) {
      if (resolved_xforms.count(dep) == 0) {
        to_satisfy.push_back(xforms_by_name_.at(dep));
        has_unresolved = true;
      }
    }
    if (has_unresolved) continue;

    // All deps are resolved. Resolve this xform.
    map<string, shared_ptr<void>> inputs;
    for (const auto &ipd: curr_xform->input_port_descriptors()) {
      auto iter = connections_to_.find({curr_xform->name(), ipd->name()});
      if (iter == connections_to_.end()) continue;
      inputs.emplace(ipd->name(), outputs_.at({iter->second}));
    }
    uint32_t err;
    std::string err_msg;
    auto out = curr_xform->apply(inputs, err, err_msg);
    if (err == XFORM_OK) {
      for (const auto &o: out) {
        outputs_.emplace(make_pair(curr_xform->name(), o.first), o.second);
      }
      resolved_xforms.emplace(curr_xform->name());
    } else {
      spdlog::error("Xfrom {} failed with code {}: {}", curr_xform->name(), err, err_msg);
      failed_xforms.emplace(curr_xform->name());
    }
    to_satisfy.pop_back();
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

std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>
XformGraph::connections() const {
  using namespace std;
  vector<pair<pair<string, string>, pair<string, string>>> conns;
  conns.reserve(connections_from_.size());
  for (auto &fcon: connections_from_) {
    conns.emplace_back(fcon);
  }
  return conns;
}

std::shared_ptr<std::pair<std::string, std::string>>
XformGraph::connection_from(const std::string &xform, const std::string &port) const {
  auto it = connections_from_.find({xform, port});
  if (it == connections_from_.end()) return nullptr;
  return std::make_shared<std::pair<std::string, std::string>>(it->second);
}

std::shared_ptr<std::pair<std::string, std::string>>
XformGraph::connection_to(const std::string &xform, const std::string &port) const {
  auto it = connections_to_.find({xform, port});
  if (it == connections_to_.end()) return nullptr;
  return std::make_shared<std::pair<std::string, std::string>>(it->second);
}

bool XformGraph::input_is_connected(const std::string &xform, const std::string &port) const {
  auto iter = connections_to_.find({xform, port});
  return !(iter == connections_to_.end());
}

bool XformGraph::output_is_connected(const std::string &xform, const std::string &port) const {
  auto iter = connections_from_.find({xform, port});
  return !(iter == connections_from_.end());
}

/*
 * We cache states and assume that each xform has a state.
 */
XformState XformGraph::state_for(const std::string &xform_name) const {
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
      auto c = connection_from(n, opd->name());
      if (c) {
        visit(c->first);
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