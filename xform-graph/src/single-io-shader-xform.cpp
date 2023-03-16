#include "single-io-shader-xform.h"
#include "xform-config.h"
#include "xform-texture-meta.h"
#include "gl_utils.h"

#include <spdlog/spdlog-inl.h>

#include <utility>

SingleIOShaderXform::SingleIOShaderXform(const std::string &name, XformConfig config) //
        : RenderXform{name, std::move(config)} //
        , shader_{nullptr} //
        , texture_id_{0} //
{
  add_input_port_descriptor("image","image",true);
  add_output_port_descriptor("image","image");
}

void SingleIOShaderXform::init() {
  spdlog::debug("SingleIOShaderXform::init");
  RenderXform::init();
  init_shader();
}

SingleIOShaderXform::~SingleIOShaderXform() {
  spdlog::debug("~SingleIOShaderXform");
  glDeleteTextures(1, &texture_id_);
}

void SingleIOShaderXform::do_init_fbo() {
  spdlog::debug("SingleIOShaderXform::do_init_fbo()");
  num_textures_ = 1;
  allocate_textures(&texture_id_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, texture_id_, 0);
}

std::map<std::string, std::shared_ptr<void>>
SingleIOShaderXform::do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
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

  resize_textures(&texture_id_, img->width, img->height);

  /*
   * Render
   */
  start_render();

  glViewport(0, 0, img->width, img->height);
  glClearColor(.3, 0, 0, 1.);
  glClear(GL_COLOR_BUFFER_BIT);

  /* Extract input image */
  glBindTexture(GL_TEXTURE_2D, img->texture_id);

  shader_->use();
  gl_check_error_and_halt("use");
  shader_->set1i("input_image", 0);
  bind_shader_variables();

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

  end_render();
  /*
   * End of Render
   */

  err = XFORM_OK;
  err_msg = "ok";
  return {{"image",
           std::static_pointer_cast<void>(
                   std::make_shared<TextureMetadata>(texture_id_, img->width, img->height))
  }};
}