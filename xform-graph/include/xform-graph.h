/*
 * An XformGraph is a directed graph of transformations. Each node is an Xform
 * Edges represent the connection of an output from one Xform to the input of another.
 * Since Xforms may have multiple outputs and inputs there may be multiple edges from one Xform
 * node to another. For example:
 *
 *
 * +------------+       +------------+
 * |  Split   R +-------+ R   Merge  |
 * | Channels G +-------+ B Channels |
 * |          B +-------+ G          |
 * +------------+       +------------+
 */
#ifndef IMAGE_TOYS_XFORM_GRAPH_H
#define IMAGE_TOYS_XFORM_GRAPH_H

#include "xform.h"

#include <string>
#include <map>
#include <set>

class XformGraph {
public:
  bool add_xform(const std::shared_ptr<Xform> &xform);

  bool add_connection(const std::string &from_xform_name,
                      const std::string &from_port,
                      const std::string &to_xform_name,
                      const std::string &to_port);

  bool evaluate() const;

  std::vector<std::shared_ptr<const Xform>> xforms() const;

  std::set<std::string>
  dependencies_for(const std::shared_ptr<Xform> &xform) const;

private:
  std::map<std::string, std::shared_ptr<Xform>> xforms_;
  std::map<std::pair<std::string, std::string>, std::pair<std::string, std::string>> connections_from_;
  std::map<std::pair<std::string, std::string>, std::pair<std::string, std::string>> connections_to_;
};

#endif //IMAGE_TOYS_XFORM_GRAPH_H
