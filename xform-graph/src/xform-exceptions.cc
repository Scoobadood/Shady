#include "xform-exceptions.h"

#include <vector>

namespace {
  const static std::vector<std::string> ERROR_MESSAGES{
    "Failed.",
    "A transform with that name already exists in the graph.",
    "No such transform in graph.",
    "No output port with that name exists on the transform.",
    "No input port with that name exists on the transform.",
    "The ports being connected are incompatible."
  };
}

std::string XformGraphException::user_error_message() {
  if( user_error_code_ >= ERROR_MESSAGES.size()) {
    return ERROR_MESSAGES[0];
  }
  return ERROR_MESSAGES[user_error_code_];
}
