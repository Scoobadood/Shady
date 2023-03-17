#ifndef IMAGE_TOYS_COMMAND_H
#define IMAGE_TOYS_COMMAND_H

#include <string>
#include <map>
#include <vector>
#include "xform-graph.h"

const int32_t CMD_NO_ERROR = 0;
const int32_t CMD_GRAPH_NOT_FOUND = 1;
const int32_t CMD_XFORM_NOT_FOUND = 2;
const int32_t CMD_PORT_NOT_FOUND = 2;
const int32_t CMD_PORTS_INCOMPATIBLE = 3;
const int32_t CMD_FAILED = 4;
const int32_t CMD_PORTS_NOT_CONNECTED = 5;
const int32_t CMD_PORTS_ALREADY_CONNECTED = 6;
const int32_t CMD_SYNTAX_ERROR = 7;
const int32_t CMD_NO_PROPERTY = 8;
const int32_t CMD_UNKNOWN_XFORM_TYPE = 9;

using Context = std::map<std::string, std::shared_ptr<void>>;

class Command {
public:
  /* Factory method */
  static std::shared_ptr<Command> from_string(const std::string &cmd);

  /*
   * Execute in the given context, possibly changing the context
   */
  virtual int32_t execute(Context &context) = 0;

  bool is_valid() const { return err_ == 0; };

  std::string err_msg() const { return err_msg_; }

  std::string output() const { return output_; }

protected:
  Command();

  virtual ~Command() = default;

  void set_error(int32_t err, const std::string &msg);

  void set_output(const std::string &msg);

  void syntax_error(const std::string &msg);

  int32_t error_graph_not_found();

  int32_t error_xform_not_found(const std::string &xform_name);

  int32_t error_port_not_found(const std::string &port_type,
                               const std::string &port_name,
                               const std::string &xform_name);

  int32_t error_no_property(const std::string &xform,
                            const std::string &config);

  int32_t error_no_such_xform(const std:: string& xform_type_);

  int32_t error_no_error();

  int32_t error_general_failure();

  static std::shared_ptr<XformGraph> get_graph(const Context &context);


private:
  int32_t err_;
  std::string err_msg_;
  std::string output_;

};

class CommandWithArgs : public Command {
public:
  explicit CommandWithArgs(const std::vector<std::string> &args) //
          : Command{} //
  {
    args_.insert(args_.end(), args.begin(), args.end());
  }

  int32_t execute(std::map<std::string, std::shared_ptr<void>> &context) override = 0;

private:
  std::vector<std::string> args_;
};


#endif //IMAGE_TOYS_COMMAND_H
