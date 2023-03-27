#include "command-save.h"
#include "xform-io.h"

Save::Save(const std::vector<std::string> &args) //
        : CommandWithArgs(args) //
{
  if (args.size() != 2) {
    set_error(91, "Syntax error. save <filename>");
    return;
  }
  file_name_ = args[1];
}

int32_t Save::execute(Context &context) {
  const std::shared_ptr<XformGraph> &graph = get_graph(context);
  if( !graph) return error_graph_not_found();

  if( save_graph(file_name_, graph) == 0) {
    return error_no_error();
  }
  return error_general_failure();
}