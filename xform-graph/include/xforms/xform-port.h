/*
 * Each Transform may have zero or more input and output ports.
 * The input ports are where the inputs to the transform are obtained
 * The output ports are wher e it publishes its results.
 * Ports are described by descriptors which determine the port name (unique to a transform)
 * the data type expected and (for InputPorts) whether the input is required.
 */
#ifndef SHADY_XFORM_PORT_H
#define SHADY_XFORM_PORT_H

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
                               bool is_required = true);

  bool is_required() const;

  const std::string &name() const;

  const std::string &data_type() const;

  bool is_compatible(const OutputPortDescriptor &other_port) const;

private:
  std::string name_;
  std::string data_type_;
  bool is_required_;
};


#endif //SHADY_XFORM_PORT_H
