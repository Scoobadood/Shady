#include "xforms/add-channel-xform.h"
#include "xform-texture-meta.h"
#include "spdlog/spdlog-inl.h"

namespace {
  const GLchar *v_shader_source[] = {R"(
#version 410 core
layout(location=0) in vec2 vtx_coord;
layout(location=1) in vec2 vtx_tex_coord;
out vec2 uv_tex_coord;

void main() {
  gl_Position = vec4(vtx_coord,0,1);
  uv_tex_coord = vtx_tex_coord;
})"};

  const GLchar *f_shader_source[] = {R"(
#version 410 core

layout (location=0) out vec4 fragColor;

in vec2 uv_tex_coord;

uniform sampler2D image_1;
uniform sampler2D image_2;

void main() {
  fragColor = texture(image_1, uv_tex_coord) + texture(image_2, uv_tex_coord);
}
)"};
}

std::string AddChannelXform::type() const {
  return TYPE;
}

AddChannelXform::AddChannelXform(const std::string &name) //
        : RenderXform(name, XformConfig{{}}) //
        , texture_id_{0}//
{
  // No single input is required but at least one must be set.
  add_input_port_descriptor("image_1", "image",true);
  add_input_port_descriptor("image_2", "image",true);
  add_output_port_descriptor("image", "image");
}

void AddChannelXform::init() {
  RenderXform::init();
  add_prog_ = std::unique_ptr<Shader>(new Shader(v_shader_source,
                                                   f_shader_source));

  if (add_prog_) is_init_ = true;
}


AddChannelXform::~AddChannelXform() {
  glDeleteTextures(1, &texture_id_);
}

std::map<std::string, std::shared_ptr<void>>
AddChannelXform::do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
                            uint32_t &err, std::string &err_msg) {
  using namespace std;

  // Bind input images
  GLuint tx_id[]{0,0};
  std::string name[]{"image_1", "image_2"};
  auto count = 0;
  auto width = -1, height = -1;
  for( auto i=0; i<2; ++i) {
    if (inputs.find(name[i]) != inputs.end()) {
      auto tm = std::static_pointer_cast<TextureMetadata>(inputs.at(name[i]));
      if (!tm) {
        err = XFORM_NULL_INPUT;
        err_msg = fmt::format("'{}' is null", name[i]);
        return {};
      }
      if( width != -1 && tm->width != width) {
        err = XFORM_MISMATCHED_SIZE;
        err_msg = fmt::format("'{}' width {} doesn't match {}", name[i], tm->width, width);
        return {};
      }
      if( height != -1 && tm->height != height) {
        err = XFORM_MISMATCHED_SIZE;
        err_msg = fmt::format("'{}' height {} doesn't match {}", name[i], tm->height, height);
        return {};
      }
      width = tm->width;
      height = tm->height;
      tx_id[i] = tm->texture_id;
      ++count;
    }
  }
  if( count != 2 ) {
    err = XFORM_MISSING_INPUT;
    err_msg = fmt::format("Must have both inputs set");
    return {};
  }

  resize_textures(1, &texture_id_, width, height);

  /*
   * Render
   */
  start_render();

  glViewport(0, 0, width, height);
  glClearColor(0, 0, 0, 1.);
  glClear(GL_COLOR_BUFFER_BIT);

  add_prog_->use();

  gl_check_error_and_halt("add_channel_xform::do_apply::add_prog->use");

  for( int i=0; i<2; ++i ) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, tx_id[i]);
    add_prog_->set_int(name[i], i);
  }
  gl_check_error_and_halt("add_channel_xform::do_apply::add_prog->set_uniforms:channels");

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

  end_render();

  gl_check_error_and_halt("add_channel_xform::do_apply");

  err = XFORM_OK;
  err_msg = "ok";
  return {{"image",
           std::static_pointer_cast<void>(std::make_shared<TextureMetadata>(texture_id_, width, height))}};
}

void
AddChannelXform::configure_framebuffer() {
  allocate_textures(1, &texture_id_);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, texture_id_, 0);
  GLenum buffs[]{GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, buffs);
}