#ifndef IMAGE_TOYS_COMMAND_CONFIGURE_H
#define IMAGE_TOYS_COMMAND_CONFIGURE_H

#include "command.h"

class Configure : public CommandWithArgs {
public:
  explicit Configure(const std::vector<std::string> &args);

  ~Configure() override = default;

  int32_t execute(Context &context) override;

private:
  std::string xform_;
  std::string config_;
  std::string value_;
};

#endif //IMAGE_TOYS_COMMAND_CONFIGURE_H
