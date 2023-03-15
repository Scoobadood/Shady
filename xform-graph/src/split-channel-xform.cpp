#include "xforms/split-channel-xform.h"
#include "xform-texture-meta.h"
#include <spdlog/spdlog-inl.h>

const GLchar *v_shader_source[] = {R"(
#version 410 core
// -0.5 - 0.5
layout(location=0) in vec2 vtx_coord;

// 0 - 1
layout(location=1) in vec2 vtx_tex_coord;


// UV coords
smooth out vec2 uv_tex_coord;

void main() {
  gl_Position = vec4(vtx_coord,0,1);
  uv_tex_coord = vtx_tex_coord;
})"};

const GLchar *f_shader_source[] = {R"(
#version 410 core

layout (location=0) out vec4 fragColor[4];

smooth in vec2 uv_tex_coord;
uniform sampler2D input_image;

void main() {
  fragColor[0] = vec4(texture( input_image, uv_tex_coord ).r);
  fragColor[1] = vec4(texture( input_image, uv_tex_coord ).g);
  fragColor[2] = vec4(texture( input_image, uv_tex_coord ).b);
  fragColor[3] = vec4(texture( input_image, uv_tex_coord ).a);
}
)"};


uint32_t SplitChannelXform::next_idx_ = 0;
const std::string TYPE = "SplitChannel";

std::string SplitChannelXform::type() const {
  return TYPE;
}

SplitChannelXform::SplitChannelXform(const std::string& name) //
        : RenderXform(name,XformConfig{{}}) //
        , texture_ids_{0, 0, 0, 0}//
{
  add_input_port_descriptor("image", "image");
  add_output_port_descriptor("red", "image");
  add_output_port_descriptor("green", "image");
  add_output_port_descriptor("blue", "image");
  add_output_port_descriptor("alpha", "image");

}

void SplitChannelXform::init() {
  RenderXform::init();
  split_prog_ = std::unique_ptr<Shader>(new Shader(v_shader_source,
                                                   (const GLchar **) nullptr,
                                                   f_shader_source));
  if( split_prog_) is_init_ = true;
}


SplitChannelXform::SplitChannelXform() //
        : SplitChannelXform(fmt::format("{}_{}", TYPE, next_idx_++)) //
{}

SplitChannelXform::~SplitChannelXform() {
  glDeleteTextures(4, texture_ids_);
}

std::map<std::string, std::shared_ptr<void>>
SplitChannelXform::do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
                            uint32_t &err, std::string &err_msg) {
  /* Extract inputs */
  auto it = inputs.find("image");
  if (it == inputs.end()) {
    err = XFORM_MISSING_INPUT;
    err_msg = fmt::format("Missing 'image'");
    return {};
  }

  auto img = std::static_pointer_cast<TextureMetadata>(it->second);
  if (!img) {
    err = XFORM_INPUT_NOT_SET;
    err_msg = fmt::format("'image' is null");
    return {};
  }

  resize_textures(4, texture_ids_, img->width, img->height);

  /*
   * Render
   */
  start_render();

  glViewport(0, 0, img->width, img->height);
  glClearColor(0, 0, 0, 1.);
  glClear(GL_COLOR_BUFFER_BIT);

  split_prog_->use();
  split_prog_->set1i("input_image", 0);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

  end_render();
  /*
   * End of Render
   */

  spdlog::info("Split");

  std::shared_ptr<void> out[4];
  for (auto i = 0; i < 4; ++i) {
    out[i] = std::static_pointer_cast<void>(
            std::make_shared<TextureMetadata>(
                    texture_ids_[i], img->width, img->height));
  }
  err = XFORM_OK;
  err_msg = "ok";
  return {
          {"red",   out[0]},
          {"green", out[1]},
          {"blue",  out[2]},
          {"alpha", out[3]}
  };
}

void
SplitChannelXform::do_init_fbo() {
  allocate_textures(4, texture_ids_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, texture_ids_[0], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                         GL_TEXTURE_2D, texture_ids_[1], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
                         GL_TEXTURE_2D, texture_ids_[2], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3,
                         GL_TEXTURE_2D, texture_ids_[3], 0);
  GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
  glDrawBuffers(4, drawBuffers);
}