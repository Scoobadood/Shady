#ifndef IMAGE_TOYS_COMMAND_DISCONNECT_H
#define IMAGE_TOYS_COMMAND_DISCONNECT_H

#include "command-root.h"

class Disconnect : public CommandWithArgs {
public:
  explicit Disconnect(const std::vector<std::string> &args);

  ~Disconnect() override = default;

  int32_t execute(Context &context) override;

private:
  std::string to_xform_;
  std::string to_port_;
};

#endif //IMAGE_TOYS_COMMAND_DISCONNECT_H
