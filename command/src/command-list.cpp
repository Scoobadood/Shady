#include "command-list.h"
#include "xform-graph.h"

#include <sstream>

int32_t List::execute(Context &context) {
  const std::shared_ptr<XformGraph> &graph = get_graph(context);
  if (!graph) return error_graph_not_found();

  std::ostringstream ss;
  if (graph->xforms().empty()) {
    ss << "  No xforms";
  } else {
    for (const auto &x: graph->xforms()) {
      ss << x->name() << "(" << x->type() << ")" << std::endl;
    }
  }
  set_output(ss.str());
  return CMD_NO_ERROR;
}
