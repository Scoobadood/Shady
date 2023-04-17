#include "command-add.h"

#include <vector>
#include "xform-factory.h"
#include "xform-exceptions.h"

// add X
// add X named NAME
Add::Add(const std::vector<std::string> &args) //
        : CommandWithArgs(args) //
{
  if (args.size() != 2 && args.size() != 4) {
    set_error(CMD_SYNTAX_ERROR, "Syntax error. add <xform> [as NAME]");
    return;
  }
  xform_type_ = args[1];
  if (args.size() == 4) {
    if (args[2] != "as") {
      set_error(CMD_SYNTAX_ERROR, "Syntax error. add <xform> [as NAME]");
      return;
    }
    xform_name_ = args[3];
  }
}

int32_t Add::execute(Context &context) {
  using namespace std;

  const std::shared_ptr<XformGraph> &graph = get_graph(context);
  if (!graph) return error_graph_not_found();

  if (!XformFactory::can_make(xform_type_)) return error_no_such_xform(xform_type_);

  auto xform = XformFactory::make_xform(xform_type_, xform_name_, {});
  if (!xform) return error_general_failure();

  try {
    graph->add_xform(xform);
    return error_no_error();
  } catch (XformGraphException &e) {
    spdlog::error("Failed to add xform with name {}, {}", xform_name_, e.what());
    return error_general_failure(e.user_error_message());
  }
}
