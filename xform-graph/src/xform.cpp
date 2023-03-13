#include "xform.h"

#include <string>
#include <utility>
#include <spdlog/spdlog-inl.h>

Xform::Xform(std::string name, XformConfig config) //
 : name_{std::move(name)} //
{
  config_ = std::move(config);
}

std::shared_ptr<const InputPortDescriptor>
Xform::input_port_descriptor_for_port(const std::string &port_name) const {
  auto it = input_port_descriptors_.find(port_name);
  if (it == input_port_descriptors_.end()) {
    spdlog::error("No input port named {} in Xform {}", port_name, name_);
    return nullptr;
  }
  return it->second;
}


void
Xform::add_input_port_descriptor(const std::string &name, const std::string &type, bool is_required) {
  if (input_port_descriptors_.find(name) != input_port_descriptors_.end()) {
    spdlog::warn("Xform {} replacing existing input port descriptor with name {}", name_, name);
  }
  auto pd = std::make_shared<InputPortDescriptor>(name, type, is_required);
  input_port_descriptors_.emplace(pd->name(), pd);
}

void
Xform::add_output_port_descriptor(const std::string &name, const std::string &type) {
  if (output_port_descriptors_.find(name) != output_port_descriptors_.end()) {
    spdlog::warn("Xform {} replacing existing output port descriptor with name {}", name_, name);
  }
  auto pd = std::make_shared<OutputPortDescriptor>(name, type);
  output_port_descriptors_.emplace(pd->name(), pd);
}

/**
 * @return the InputPortsDescriptors for this xform
 */
std::vector<std::shared_ptr<const InputPortDescriptor>>
Xform::input_port_descriptors() const {
  using namespace std;
  vector<shared_ptr<const InputPortDescriptor>> values;
  for( const auto & e : input_port_descriptors_) {
    values.emplace_back(e.second);
  }
  return values;
}


/**
 * @param port_name
 * @return The OutputPortDescriptor for the given port or nullptr if not found.
 */
std::shared_ptr<const OutputPortDescriptor>
Xform::output_port_descriptor_for_port(const std::string &port_name) const {
  auto it = output_port_descriptors_.find(port_name);
  if (it == output_port_descriptors_.end()) {
    spdlog::error("No output port named {} in Xform {}", port_name, name_);
    return nullptr;
  }
  return it->second;
}


const std::string &Xform::name() {
  return name_;
}


/**
 * Apply the transform to its inputs and make the result
 * available to outputs.
 * @param inputs A map from input port names to inputs.
 * @return a map from output port names to outputs.
 */
std::map<std::string, void *>
Xform::apply(const std::map<std::string, void *> &inputs) {
  /* Will throw if an input is invalid */
  for (const auto &ipd: input_port_descriptors_) {
    validate(ipd.second, inputs);
  }

  return do_apply(inputs);
}

void
Xform::validate(const std::shared_ptr<const InputPortDescriptor> &ipd,
                const std::map<std::string, void *> &inputs) {
  auto iter = inputs.find(ipd->name());
  if (ipd->is_required() && iter == inputs.end()) {
    auto err = fmt::format("Missing input: {} in transform {}", ipd->name(), name_);
    spdlog::critical(err);
    throw std::runtime_error(err);
  }
}

