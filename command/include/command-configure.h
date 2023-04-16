#ifndef SHADY_COMMAND_CONFIGURE_H
#define SHADY_COMMAND_CONFIGURE_H

#include "command-root.h"

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

#endif //SHADY_COMMAND_CONFIGURE_H
