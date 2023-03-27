#include "command-root.h"
#include "string_utils.h"
#include "command-load.h"
#include "command-list.h"
#include "command-save.h"
#include "command-show.h"
#include "command-quit.h"
#include "command-connect.h"
#include "command-delete.h"
#include "command-run.h"
#include "command-disconnect.h"
#include "command-configure.h"
#include "command-add.h"

#include <string>
#include <map>

#include <spdlog/spdlog-inl.h>

Command::Command() //
        : err_{0} //
{}

int32_t Command::error_graph_not_found() {
  set_output("No graph.");
  return CMD_GRAPH_NOT_FOUND;
}

int32_t Command::error_xform_not_found(const std::string &xform_name) {
  set_output(fmt::format("Xform {} not found", xform_name));
  return CMD_XFORM_NOT_FOUND;
}

int32_t Command::error_port_not_found(const std::string &port_type,
                                      const std::string &port_name,
                                      const std::string &xform_name
) {
  set_output(fmt::format("{} port {} not found on xform {}", port_type, port_name, xform_name));
  return CMD_PORT_NOT_FOUND;
}

int32_t Command::error_no_property(const std::string &xform, const std::string &config) {
  set_output(fmt::format("Xform {} does not have a property called {}", xform, config));
  return CMD_NO_PROPERTY;
}

int32_t Command::error_no_error(){
  set_output("OK");
  return CMD_NO_ERROR;
}

int32_t Command::error_general_failure(){
  set_output("Failed");
  return CMD_FAILED;
}

int32_t Command::error_no_such_xform(const std:: string& xform_type_) {
  set_output(fmt::format("Invalid xform: {}", xform_type_));
  return CMD_UNKNOWN_XFORM_TYPE;
}


std::shared_ptr<XformGraph> Command::get_graph(const Context &context) {
  auto it = context.find("graph");
  if (it == context.end() || it->second == nullptr) return nullptr;
  return std::static_pointer_cast<XformGraph>(it->second);
}

void Command::set_error(int32_t err, const std::string &msg) {
  err_ = err;
  err_msg_ = msg;
}

void Command::syntax_error(const std::string &msg) {
  err_ = CMD_SYNTAX_ERROR;
  err_msg_ = msg;
}

void Command::set_output(const std::string &msg) {
  output_ = msg;
}

std::shared_ptr<Command> Command::from_string(const std::string &cmd) {
  using namespace std;

  auto cmd_strings = split(cmd, ' ');
  if (cmd_strings.empty()) return nullptr;
  std::for_each(cmd_strings.front().begin(), cmd_strings.front().end(),
                [](char c) { tolower(c); });
  auto cmd_name = cmd_strings.front();

  if (cmd_name == "quit") return make_shared<Quit>();
  if (cmd_name == "load") return make_shared<Load>(cmd_strings);
  if (cmd_name == "save") return make_shared<Save>(cmd_strings);
  if (cmd_name == "list") return make_shared<List>(cmd_strings);
  if (cmd_name == "show") return make_shared<Show>(cmd_strings);
  if (cmd_name == "connect") return make_shared<Connect>(cmd_strings);
  if (cmd_name == "disconnect") return make_shared<Disconnect>(cmd_strings);
  if (cmd_name == "configure") return make_shared<Configure>(cmd_strings);
  if (cmd_name == "delete") return make_shared<Delete>(cmd_strings);
  if (cmd_name == "run") return make_shared<Run>();
  if (cmd_name == "add") return make_shared<Add>(cmd_strings);
  return nullptr;
}