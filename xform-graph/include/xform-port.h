/*
 * Transforms have input and output ports.
 * These are described by descriptors which determine their
 * names, whether they are required and other characteristics.
 */
#ifndef IMAGE_TOYS_XFORM_PORT_H
#define IMAGE_TOYS_XFORM_PORT_H

#include <string>

class OutputPortDescriptor {
public:
  explicit OutputPortDescriptor(std::string name, std::string data_type);

  const std::string &name() const;

  const std::string &data_type() const;

private:
  std::string name_;
  std::string data_type_;
};


class InputPortDescriptor {
public:
  explicit InputPortDescriptor(std::string name,
                               std::string data_type,
                               bool is_required = false);

  bool is_required() const;

  const std::string &name() const;

  const std::string &data_type() const;

  bool is_compatible(const OutputPortDescriptor &other_port) const;

private:
  std::string name_;
  std::string data_type_;
  bool is_required_;
};


#endif //IMAGE_TOYS_XFORM_PORT_H
