#include "command-load.h"

Load::Load(const std::vector<std::string> &args) //
        : CommandWithArgs(args) //
{
  if (args.size() != 2) {
    set_error(11, "Syntax error. load <filename>");
    return;
  }
  file_name_ = args[1];
}

int32_t Load::execute(Context &context) {
  auto graph = load_graph(file_name_);
  if (graph) {
    context["graph"] = graph;
    return error_no_error();
  }
  return error_general_failure();
}