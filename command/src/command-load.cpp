#include "command-load.h"
#include "xform-exceptions.h"

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
  try {
    auto graph = load_graph(file_name_);
    context["graph"] = graph;
    return error_no_error();
  } catch (XformGraphException &e) {
    spdlog::error("Failed to load graph from :{}. Cause: {}",
                  file_name_,
                  e.what());
    return error_general_failure(e.user_error_message());
  }
}