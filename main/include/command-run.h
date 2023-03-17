#ifndef IMAGE_TOYS_COMMAND_RUN_H
#define IMAGE_TOYS_COMMAND_RUN_H

#include "command.h"

class Run : public Command {
public:
  ~Run() override = default;

  int32_t execute(std::map<std::string, std::shared_ptr<void>> &context) override;
};

#endif //IMAGE_TOYS_COMMAND_RUN_H
