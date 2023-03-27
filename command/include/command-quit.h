#ifndef IMAGE_TOYS_COMMAND_QUIT_H
#define IMAGE_TOYS_COMMAND_QUIT_H

#include "command-root.h"

class Quit : public Command {
public:
  int32_t execute(Context &context) override;

  ~Quit() override = default;
};



#endif //IMAGE_TOYS_COMMAND_QUIT_H
