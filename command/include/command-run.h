#ifndef SHADY_COMMAND_RUN_H
#define SHADY_COMMAND_RUN_H

#include "command-root.h"

class Run : public Command {
public:
  ~Run() override = default;

  int32_t execute(std::map<std::string, std::shared_ptr<void>> &context) override;
};

#endif //SHADY_COMMAND_RUN_H
