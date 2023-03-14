#include "xform.h"
#include "gl_utils.h"

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
  for (const auto &e: input_port_descriptors_) {
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

const std::string &Xform::name() const {
  return name_;
}


/**
 * Apply the transform to its inputs and make the result
 * available to outputs.
 * @param inputs A map from input port names to inputs.
 * @return a map from output port names to outputs.
 */
std::map<std::string, std::shared_ptr<void>>
Xform::apply(const std::map<std::string, std::shared_ptr<void>> &inputs, uint32_t &err, std::string &err_msg) {
  for (const auto &ipd: input_port_descriptors_) {
    auto it = inputs.find(ipd.first);
    if (ipd.second->is_required() && it == inputs.end()) {
      err = XFORM_MISSING_INPUT;
      err_msg =fmt::format("Missing input: {} in transform {}", ipd.first, name_);
      return {};
    }
  }
  err = XFORM_OK;
  err_msg = "OK";
  return do_apply(inputs, err, err_msg);
}

void
Xform::allocate_textures(int32_t n, GLuint *texture_ids) {
  glGenTextures(n, texture_ids);
  for (auto i = 0; i < n; ++i) {
    if (texture_ids[i] == 0) {
      spdlog::error("Xfrom allocate_textures() got invalid texture ID");
      return;
    }
  }

  glActiveTexture(GL_TEXTURE0);
  for (auto i = 0; i < n; ++i) {
    glBindTexture(GL_TEXTURE_2D, texture_ids[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 10, 10, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  }
  gl_check_error_and_halt("generate_texture");
}

void
Xform::resize_textures(uint32_t n, GLuint *texture_ids, GLsizei width, GLsizei height) {
  glActiveTexture(GL_TEXTURE0);
  for (auto i = 0; i < n; ++i) {
    glBindTexture(GL_TEXTURE_2D, texture_ids[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  }
  gl_check_error_and_halt("generate_texture");
}
