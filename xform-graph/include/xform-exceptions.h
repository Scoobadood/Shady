#ifndef SHADY_XFORM_EXCEPTIONS_H
#define SHADY_XFORM_EXCEPTIONS_H

#include <string>
#include <cstdint>

const uint32_t ERR_GENERAL_FAILURE = 0;
const uint32_t ERR_XFORM_EXISTS = 1;
const uint32_t ERR_NO_SUCH_XFORM = 2;
const uint32_t ERR_NO_SUCH_OUTPUT_PORT = 3;
const uint32_t ERR_NO_SUCH_INPUT_PORT = 4;
const uint32_t ERR_PORTS_ARE_INCOMPATIBLE = 5;


class XformGraphException : public std::runtime_error {
public:
  explicit XformGraphException(const std::string &msg, uint32_t error_code = ERR_GENERAL_FAILURE) //
          : std::runtime_error(msg) //
          , user_error_code_{error_code}
  {}

  std::string user_error_message();

  inline uint32_t user_error_code() const { return user_error_code_; }

private:
  uint32_t user_error_code_;
};


#endif //SHADY_XFORM_EXCEPTIONS_H
