#include "command-delete.h"
#include "xform-graph.h"
#include "xform-exceptions.h"

#include "spdlog/spdlog-inl.h"

Delete::Delete(const std::vector<std::string> &args) //
        : CommandWithArgs(args) //
{
  if (args.size() != 2) {
    set_error(71, "Syntax error. delete <xform>");
    return;
  }
  xform_name_ = args[1];
}

int32_t Delete::execute(Context &context) {
  using namespace std;

  const std::shared_ptr<XformGraph> &graph = get_graph(context);
  if (!graph) return error_graph_not_found();

  auto xform = graph->xform(xform_name_);
  if (!xform) return error_xform_not_found(xform_name_);

  try {
    graph->delete_xform(xform_name_);
    return error_no_error();
  } catch (XformGraphException &e) {
    spdlog::error("Delete xform {} failed. Cause: {}",
                  xform_name_, e.what());
    return error_general_failure(e.user_error_message());
  }
}