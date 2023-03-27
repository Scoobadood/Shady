#include "xform-graph.h"
#include "command-show.h"

#include <sstream>

Show::Show(const std::vector<std::string> &args) //
        : CommandWithArgs(args) //
{
  if (args.size() != 2) {
    set_error(31, "Syntax error. show <xform>");
    return;
  }
  xform_name_ = args[1];
}

int32_t Show::execute(Context &context) {
  using namespace std;

  const std::shared_ptr<XformGraph> &graph = get_graph(context);
  if( !graph) return error_graph_not_found();

  ostringstream ss;

  bool found = false;
  for (const auto &x: graph->xforms()) {
    if (x->name() != xform_name_) continue;

    found = true;
    ss << x->name() << "(" << x->type() << ")" << endl;
    ss << " Config" << endl;
    if (x->config().descriptors().empty())
      ss << "  None" << endl;

    for (const auto &pd: x->config().descriptors()) {
      ss << "  " << pd.name << "(" << pd.type_name() << ")";
      switch (pd.type) {
        case XformConfig::PropertyDescriptor::STRING: {
          string value;
          if (x->config().get(pd.name, value)) {
            ss << "=" << value << endl;
          }
          break;
        }

        case XformConfig::PropertyDescriptor::FLOAT: {
          float value;
          if (x->config().get(pd.name, value)) {
            ss << "=" << value << endl;
          }
          break;
        }

        case XformConfig::PropertyDescriptor::INT: {
          int value;
          if (x->config().get(pd.name, value)) {
            ss << "=" << value << endl;
          }
          break;
        }

        default:
          ss << "=" << "???" << endl;
          break;
      }
    }
    ss << endl;


    ss << " Inputs" << std::endl;
    for (const auto &ipd: x->input_port_descriptors()) {
      ss << "  " << ipd->name() << "(" << ipd->data_type() << ") "
         << (ipd->is_required() ? "Req" : "   ");

      auto to = graph->connection_to(x->name(), ipd->name());
      if (to) ss << ", connected from " << to->first << ":" << to->second;
      ss << endl;
    }
    ss << endl;


    ss << " Outputs" << std::endl;
    for (const auto &opd: x->output_port_descriptors()) {
      ss << "  " << opd->name() << "(" << opd->data_type() << ") ";

      auto from = graph->connection_from(x->name(), opd->name());
      if (from) ss << ", connected to " << from->first << ":" << from->second;
      ss << endl;
    }

    set_output(ss.str());
  }

  if (!found) {
    set_output("Couldn't find xform " + xform_name_);
    return 32;
  }

  return 0;
}
