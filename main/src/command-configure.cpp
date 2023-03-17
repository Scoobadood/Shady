#include "command-configure.h"

// configure <xform> set <config> <value>
Configure::Configure(const std::vector<std::string> &args) //
        : CommandWithArgs(args) //
{
  if (args.size() != 5 || args[2] != "set") {
    syntax_error("Syntax error. configure <xform> set <config> <value>");
    return;
  }
  xform_ = args[1];
  config_ = args[3];
  value_ = args[4];
}

int32_t Configure::execute(Context &context) {
  const std::shared_ptr<XformGraph> &graph = get_graph(context);
  if (!graph) return error_graph_not_found();

  auto xform = graph->xform(xform_);
  if (!xform) return error_xform_not_found(xform_);

  const XformConfig::PropertyDescriptor * pd = nullptr;
  for( const auto & apd : xform->config().descriptors()) {
    if( apd.name == config_ ) {
      pd = &apd;
      break;
    }
  }
  if( !pd) {
    return error_no_property(xform_, config_);
  }

  switch(pd->type) {
    case XformConfig::PropertyDescriptor::STRING:
      xform->config().set(config_, value_);
      break;
    case XformConfig::PropertyDescriptor::FLOAT:
      xform->config().set(config_, stof(value_));
      break;
    case XformConfig::PropertyDescriptor::INT:
      xform->config().set(config_, stoi(value_));
      break;
    default:
      return error_general_failure();
  }

  return error_no_error();
}