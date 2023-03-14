/*
 * A transformation.
 * Takes one or more inputs of various image types and operates on them to produce one or more outputs.
 * The inputs may have limitation on certain characteristics of the image
 * such as (colour depth, number of channels, dimensions etc.)
 * Some transformations are sources; they have no inputs but produce a single output.
 * Some are Sinks; they consume one or more inputs but produce no output.
 * Some may have multiple inputs, others multiple outputs.
 * They all have a Configuration which consists of a set of name value parameter pairs.
 */
#ifndef IMAGE_TOYS_XFORM_H
#define IMAGE_TOYS_XFORM_H

#include "xform-port.h"
#include "xform-config.h"

#include <string>
#include <map>
#include <vector>


#include <OpenGL/gl3.h>

const uint32_t XFORM_OK = 0;
const uint32_t XFORM_MISSING_CONFIG = 1;
const uint32_t XFORM_INVALID_CONFIG = 2;
const uint32_t XFORM_MISSING_INPUT = 3;
const uint32_t XFORM_INPUT_NOT_SET = 4;


class Xform {
public:
  /**
 * Apply the transform to its inputs and make the result
 * available to outputs.
 * @param inputs A map from input port names to inputs.
 * @return a map from output port names to outputs.
 */
  std::map<std::string, std::shared_ptr<void>>
  apply(const std::map<std::string, std::shared_ptr<void>> &inputs, uint32_t &err, std::string &err_msg);

  /**
   * Get a mutable reference to the config.
   */
   XformConfig& config() {return config_;}

  /**
   * @return The unique name for this transform.
   */
  const std::string &name();

  /**
   * @return true if there are no outputs for this xform.
   */
  bool inline is_sink() const {
    return output_port_descriptors_.empty();
  }

  /**
   * @return true if there are no inputs for this xform.
   */
  bool inline is_source() const {
    return input_port_descriptors_.empty();
  }

  /**
   * @return the InputPortsDescriptors for this xform
   */
   std::vector<std::shared_ptr<const InputPortDescriptor>>
   input_port_descriptors() const;

  /**
   * @param port_name
   * @return The InputPortDescriptor for the given port or nullptr if not found.
   */
  std::shared_ptr<const InputPortDescriptor>
  input_port_descriptor_for_port(const std::string &port_name) const;

/**
 * @param port_name
 * @return The OutputPortDescriptor for the given port or nullptr if not found.
 */
  std::shared_ptr<const OutputPortDescriptor>
  output_port_descriptor_for_port(const std::string &port_name) const;

protected:
  explicit Xform(std::string name, XformConfig config= XformConfig{});
  virtual ~Xform() = default;

  static void allocate_textures(int32_t n, GLuint * texture_ids);
  static void resize_textures( uint32_t n, GLuint * texture_ids, GLsizei width, GLsizei height);

  void add_input_port_descriptor(const std::string& name, const std::string& type, bool is_required = true);
  void add_output_port_descriptor(const std::string& name, const std::string& type );
  std::map<std::string, std::shared_ptr<const InputPortDescriptor>> input_port_descriptors_;
  std::map<std::string, std::shared_ptr<const OutputPortDescriptor>> output_port_descriptors_;

private:
  void validate(const std::shared_ptr<const InputPortDescriptor> &ipd,
                const std::map<std::string, std::shared_ptr<void>> &inputs);

  virtual std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs, uint32_t &err, std::string &err_msg) = 0;

  std::string name_;

  XformConfig config_;
};


#endif //IMAGE_TOYS_XFORM_H
