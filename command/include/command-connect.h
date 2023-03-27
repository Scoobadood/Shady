#ifndef IMAGE_TOYS_COMMAND_CONNECT_H
#define IMAGE_TOYS_COMMAND_CONNECT_H

#include "command-root.h"

class Connect : public CommandWithArgs {
public:
  explicit Connect(const std::vector<std::string> &args);

  ~Connect() override = default;

  int32_t execute(Context &context) override;

private:
  std::string from_xform_;
  std::string from_port_;
  std::string to_xform_;
  std::string to_port_;
};

#endif //IMAGE_TOYS_COMMAND_CONNECT_H
