#include "xform-port.h"

#include <string>

InputPortDescriptor::InputPortDescriptor(std::string name,
                                         std::string data_type,
                                         bool is_required) //
        : name_{std::move(name)} //
        , data_type_{std::move(data_type)} //
        , is_required_{is_required} //
{}


bool
InputPortDescriptor::is_required() const {
  return is_required_;
}

const std::string &
InputPortDescriptor::name() const {
  return name_;
}

const std::string &
InputPortDescriptor::data_type() const {
  return data_type_;
}

bool
InputPortDescriptor::is_compatible(const OutputPortDescriptor &other_port) const {
  return true;
}

OutputPortDescriptor::OutputPortDescriptor(std::string name, std::string data_type) //
        : name_{std::move(name)} //
        , data_type_{std::move(data_type)} //
{}


const std::string &
OutputPortDescriptor::name() const {
  return name_;
}

const std::string &
OutputPortDescriptor::data_type() const {
  return data_type_;
}
