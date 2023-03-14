#include "xform-graph.h"
#include "xform.h"

#include <set>
#include <map>
#include <deque>

#include <spdlog/spdlog-inl.h>

bool
XformGraph::add_xform(const std::shared_ptr<Xform> &xform) {
  if (xforms_.find(xform->name()) != xforms_.end()) {
    spdlog::error("Xform {} already in graph.", xform->name());
    return false;
  }

  xforms_.emplace(xform->name(), xform);
  return true;
}

bool XformGraph::add_connection(const std::string &from_xform_name,
                                const std::string &from_port_name,
                                const std::string &to_xform_name,
                                const std::string &to_port_name) {
  auto it = xforms_.find(from_xform_name);
  if (it == xforms_.end()) {
    spdlog::error("No such xform: {}", from_xform_name);
    return false;
  }
  auto from_xform = it->second;

  it = xforms_.find(to_xform_name);
  if (it == xforms_.end()) {
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
 * If we evaluate all xforms and the sinks are still unsatsfied
 * return false otherwise return true.
 * TODO: We want to be lazy abou this and not force re-evaluate things that are still
 *       valid. We need to decide if we have xforms cache results or do it in the graph.
 *       For now we will re-evaluate everything.
 */
bool XformGraph::evaluate() const {
  using namespace std;

  /*
   * Some xforms may not have mandatory inputs connected.
   * If so, they cannot be evaluated. Flag them as failed first.
   */
  set<string> failed_xforms;
  for (const auto &xform: xforms_) {
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
  for (const auto &xform: xforms_) {
    if (xform.second->is_sink()) {
      to_satisfy.push_back(xform.second);
    }
  }

  // Evaluate until we're done or cannot continue.
  set<string> resolved_xforms;
  map<pair<string, string>, shared_ptr<void>> results;
  while (!to_satisfy.empty()) {
    const auto &curr_xform = to_satisfy.back();

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
        to_satisfy.push_back(xforms_.at(dep));
        has_unresolved = true;
      }
    }
    if (has_unresolved) continue;

    // All deps are resolved. Resolve this xform.
    map<string, shared_ptr<void>> inputs;
    for (const auto &ipd: curr_xform->input_port_descriptors()) {
      auto iter = connections_to_.find({curr_xform->name(), ipd->name()});
      if (iter == connections_to_.end()) continue;
      inputs.emplace(ipd->name(), results.at({iter->second}));
    }
    uint32_t err;
    std::string err_msg;
    auto out = curr_xform->apply(inputs, err, err_msg);
    if (err == XFORM_OK) {
      for (const auto &o: out) {
        results.emplace(make_pair(curr_xform->name(), o.first), o.second);
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
  for (auto &e: xforms_) {
    xforms.push_back(e.second);
  }
  return xforms;
}
