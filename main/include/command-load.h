//
// Created by Dave Durbin on 17/3/2023.
//

#ifndef IMAGE_TOYS_COMMAND_LOAD_H
#define IMAGE_TOYS_COMMAND_LOAD_H

#include "command.h"
#include "xform-io.h"

class Load : public CommandWithArgs {
public:
  explicit Load(const std::vector<std::string> &args);

  ~Load() override = default;

  int32_t execute(Context &context) override;

private:
  std::string file_name_;
};

#endif //IMAGE_TOYS_COMMAND_LOAD_H
