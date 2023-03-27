#include "command-connect.h"
#include "xform-graph.h"

#include "spdlog/spdlog-inl.h"

Connect::Connect(const std::vector<std::string> &args) //
        : CommandWithArgs(args) //
{
  if (args.size() != 5) {
    set_error(41, "Syntax error. connect <from xform> <from port> <to xform> <to port>");
    return;
  }
  from_xform_ = args[1];
  from_port_ = args[2];
  to_xform_ = args[3];
  to_port_ = args[4];
}

int32_t Connect::execute(Context &context) {
  const std::shared_ptr<XformGraph> &graph = get_graph(context);
  if (!graph) return error_graph_not_found();

  auto from = graph->xform(from_xform_);
  if (!from) return error_xform_not_found(from_xform_);

  auto to = graph->xform(to_xform_);
  if (!to) return error_xform_not_found(to_xform_);

  auto from_pd = from->output_port_descriptor_for_port(from_port_);
  if (!from_pd) return error_port_not_found("output", from_xform_, from_port_);

  auto to_pd = to->input_port_descriptor_for_port(to_port_);
  if (!to_pd) return error_port_not_found("input", to_xform_, to_port_);

  if (!to_pd->is_compatible(*from_pd)) {
    set_output(fmt::format("Ports {}:{} and {}:{} are incompatible",
                           from_xform_, from_port_, to_xform_, to_port_));
    return CMD_PORTS_INCOMPATIBLE;
  }

  auto fm = graph->connection_to(to_xform_, to_port_);
  if (fm && fm->first == from_xform_ && fm->second == from_port_) {
    set_output(fmt::format("{}:{} is already connected to {}:{}",
                           from_xform_, from_port_, to_xform_, to_port_));
    return CMD_PORTS_ALREADY_CONNECTED;
  }

  if (!graph->add_connection(from_xform_, from_port_, to_xform_, to_port_)) {
    return error_general_failure();
  }
  return error_no_error();
}