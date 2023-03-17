#include "command-run.h"
#include "xform-graph.h"

int32_t Run::execute(Context &context) {
  const std::shared_ptr<XformGraph> &graph = get_graph(context);
  if( !graph) return error_graph_not_found();

  if( graph->evaluate() ) {
    return error_no_error();
  }
  return error_general_failure();
}