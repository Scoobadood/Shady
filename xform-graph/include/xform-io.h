/**
 * Handle IO for Graphs. Provides a means to save to and load from text files.
 * See docs for text file format.
 */
#ifndef IMAGE_TOYS_XFORM_IO_H
#define IMAGE_TOYS_XFORM_IO_H

#include "xform-graph.h"
#include <cstdint>

int32_t
xform_write_graph(std::ostream &os, const std::shared_ptr<XformGraph> &graph);

std::shared_ptr<XformGraph>
xform_read_graph(std::istream &is);

int32_t
save_graph(const std::string &file_name, const std::shared_ptr<XformGraph> &graph);

std::shared_ptr<XformGraph>
load_graph(const std::string &file_name);

#endif //IMAGE_TOYS_XFORM_IO_H
