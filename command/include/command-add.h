#ifndef SHADY_COMMAND_ADD_H
#define SHADY_COMMAND_ADD_H

#include "command-root.h"

class Add : public CommandWithArgs {
public:
  explicit Add(const std::vector<std::string> &args);

  ~Add() override = default;

  int32_t execute(Context &context) override;

private:
  std::string xform_name_;
  std::string xform_type_;
};

#endif //SHADY_COMMAND_ADD_H
