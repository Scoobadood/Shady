#include "load-file-xform.h"
#include "split-channel-xform.h"
#include "save-file-xform.h"
#include "xform-graph.h"

#include <GLFW/glfw3.h>

GLFWwindow *initgl(){
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);

  // Create window with graphics context
  auto window = glfwCreateWindow(100, 100, "", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  return window;
}


int main( ) {
  auto win = initgl();

  using namespace std;


  XformGraph g;
  auto load = make_shared<LoadFileXform>();
  g.add_xform(load);
  load->config().set("file_name", "/Users/dave/CLionProjects/image_toys/data/parrot.png");

  auto split = make_shared<SplitChannelXform>();
  g.add_xform(split);

  auto save_r = make_shared<SaveFileXform>();
  g.add_xform(save_r);
  save_r->config().set("file_name", "/Users/dave/CLionProjects/image_toys/data/parrot_red.png");
  auto save_g = make_shared<SaveFileXform>();
  g.add_xform(save_g);
  save_g->config().set("file_name", "/Users/dave/CLionProjects/image_toys/data/parrot_grn.png");
  auto save_b = make_shared<SaveFileXform>();
  g.add_xform(save_b);
  save_b->config().set("file_name", "/Users/dave/CLionProjects/image_toys/data/parrot_blu.png");
  auto save_a = make_shared<SaveFileXform>();
  g.add_xform(save_a);
  save_a->config().set("file_name", "/Users/dave/CLionProjects/image_toys/data/parrot_alf.png");

  g.add_connection("LoadFile_0", "image",
                   "SplitChannel_0", "image");

  g.add_connection("SplitChannel_0", "red",
                   "SaveFile_0", "image");
  g.add_connection("SplitChannel_0", "green",
                   "SaveFile_1", "image");
  g.add_connection("SplitChannel_0", "blue",
                   "SaveFile_2", "image");
  g.add_connection("SplitChannel_0", "alpha",
                   "SaveFile_3", "image");

  g.evaluate();

  glfwDestroyWindow(win);
  glfwTerminate();
}