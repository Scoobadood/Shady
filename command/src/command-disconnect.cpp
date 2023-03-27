#include "command-disconnect.h"
#include "xform-graph.h"

#include "spdlog/spdlog-inl.h"

Disconnect::Disconnect(const std::vector<std::string> &args) //
        : CommandWithArgs(args) //
{
  if (args.size() != 3) {
    set_error(101, "Syntax error. disconnect <to xform> <to port>");
    return;
  }
  to_xform_ = args[1];
  to_port_ = args[2];
}

int32_t Disconnect::execute(Context &context) {
  const std::shared_ptr<XformGraph> &graph = get_graph(context);
  if (!graph) return error_graph_not_found();

  auto to = graph->xform(to_xform_);
  if (!to) return error_xform_not_found(to_xform_);

  auto to_pd = to->input_port_descriptor_for_port(to_port_);
  if (!to_pd) return error_port_not_found("input", to_xform_, to_port_);


  auto from =graph->connection_to(to_xform_, to_port_);
  if( !from) {
    set_output(fmt::format("{}:{} is not connected",
                           to_xform_, to_port_));
    return CMD_PORTS_NOT_CONNECTED;
  }

  if (graph->remove_connection(to_xform_, to_port_)) {
    return error_no_error();
  }
  return error_general_failure();
}