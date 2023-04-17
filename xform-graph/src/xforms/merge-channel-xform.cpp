/*
 * Given optional RGBA inputs - or colours for missing inputs -
 * merge the channels together to form a single RGBA image output.
 *
 * The configuration allows the user to specify a single colour (0.0 = 1.0) for
 * any channel that doesn't have an niput
 *
 * A colour specified for an channel with an input is just ignored.
 */


#include "xforms/merge-channel-xform.h"

#include "xform-texture-meta.h"
#include "spdlog/spdlog-inl.h"

namespace {
  const GLchar *v_shader_source[] = {R"(
#version 410 core
layout(location=0) in vec2 vtx_coord;
layout(location=1) in vec2 vtx_tex_coord;
smooth out vec2 uv_tex_coord;

void main() {
  gl_Position = vec4(vtx_coord,0,1);
  uv_tex_coord = vtx_tex_coord;
})"};

  const GLchar *f_shader_source[] = {R"(
#version 410 core

layout (location=0) out vec4 fragColor;

smooth in vec2 uv_tex_coord;

uniform sampler2D channel_red;
uniform sampler2D channel_green;
uniform sampler2D channel_blue;
uniform sampler2D channel_alpha;
uniform bvec4 channel_used;

void main() {
  float r = channel_used[0] ?texture(channel_red, uv_tex_coord).r : 0;
  float g = channel_used[1] ?texture(channel_green, uv_tex_coord).g : 0;
  float b = channel_used[2] ?texture(channel_blue, uv_tex_coord).b : 0;
  float a = channel_used[3] ?texture(channel_alpha, uv_tex_coord).a : 0;
  fragColor = vec4(r,g,b,a);
}
)"};
}

std::string MergeChannelXform::type() const {
  return TYPE;
}

MergeChannelXform::MergeChannelXform(const std::string &name) //
        : RenderXform(name, XformConfig{{}}) //
        , texture_id_{0}//
{
  // No single input is required but at least one must be set.
  add_input_port_descriptor("red", "image",false);
  add_input_port_descriptor("green", "image",false);
  add_input_port_descriptor("blue", "image",false);
  add_input_port_descriptor("alpha", "image",false);
  add_output_port_descriptor("image", "image");

}

void MergeChannelXform::init() {
  RenderXform::init();
  merge_prog_ = std::unique_ptr<Shader>(new Shader(v_shader_source,
                                                   f_shader_source));

  if (merge_prog_) is_init_ = true;
}


MergeChannelXform::~MergeChannelXform() {
  glDeleteTextures(1, &texture_id_);
}

std::map<std::string, std::shared_ptr<void>>
MergeChannelXform::do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
                            uint32_t &err, std::string &err_msg) {
  using namespace std;

  GLuint tx_id[]{0,0,0,0};
  bool input_present[]{false, false, false, false};
  std::string name[]{"red", "green", "blue", "alpha"};
  auto count = 0;
  auto width = -1, height = -1;
  for( auto i=0; i<4; ++i) {
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
      input_present[i] = true;
      tx_id[i] = tm->texture_id;
      ++count;
    }
  }
  if( count == 0 ) {
    err = XFORM_MISSING_INPUT;
    err_msg = fmt::format("Must have at least one input set");
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

  merge_prog_->use();

  gl_check_error_and_halt("merge_channel_xform::do_apply::merged_prog->use");

  for( int i=0; i<4; ++i ) {
    glActiveTexture(GL_TEXTURE0 + i);
    if( input_present[i] ) {
      glBindTexture(GL_TEXTURE_2D, tx_id[i]);
      auto un = fmt::format("channel_{}", name[i]);
      merge_prog_->set_int(un, i);
    }
  }
  gl_check_error_and_halt("merge_channel_xform::do_apply::merged_prog->set_uniforms:channels");

  merge_prog_->set4b("channel_used", input_present);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

  end_render();

  gl_check_error_and_halt("merge_channel_xform::do_apply");

  err = XFORM_OK;
  err_msg = "ok";
  return {{"image",
           std::static_pointer_cast<void>(std::make_shared<TextureMetadata>(texture_id_, width, height))}};
}

void
MergeChannelXform::configure_framebuffer() {
  allocate_textures(1, &texture_id_);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, texture_id_, 0);
  GLenum buffs[]{GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, buffs);
}