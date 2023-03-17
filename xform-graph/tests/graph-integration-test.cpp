#include "xform-graph.h"
#include "xforms/xform.h"
#include "xforms/load-file-xform.h"
#include "xforms/split-channel-xform.h"
#include "xforms/save-file-xform.h"

int main() {
  XformGraph g;
  auto load = std::make_shared<LoadFileXform>();
  g.add_xform(load);
  load->config().set("file_name", "/Users/dave/CLionProjects/image_toys/data/parrot.png");

  auto split = std::make_shared<SplitChannelXform>();
  g.add_xform(split);

  auto save = std::make_shared<SaveFileXform>();
  g.add_xform(save);
  save->config().set("file_name", "/Users/dave/CLionProjects/image_toys/data/parrot_out.png");

  g.add_connection("LoadFile_0", "image",
                   "SplitChannel_0", "image");

  g.add_connection("SplitChannel_0", "red",
                   "SaveFile_0", "image");

  g.evaluate();

}