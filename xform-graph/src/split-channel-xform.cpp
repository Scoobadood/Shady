#include "split-channel-xform.h"

#include <spdlog/spdlog-inl.h>
#include "xform-texture-meta.h"

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

layout (location=0) out vec4 vFragColor[4];

smooth in vec2 uv_tex_coord;
uniform sampler2D input_image;

void main() {
  fragColor[0] = vec4(texture( input_image, uv_tex_coord ).r);
  fragColor[1] = vec4(texture( input_image, uv_tex_coord ).g);
  fragColor[2] = vec4(texture( input_image, uv_tex_coord ).b);
  fragColor[3] = vec4(texture( input_image, uv_tex_coord ).a);
}
)"};


void init_buffers(GLuint &vao_id, GLuint &vbo_verts, GLuint &vbo_indices);

uint32_t SplitChannelXform::next_idx_ = 0;

SplitChannelXform::SplitChannelXform() //
        : Xform("SplitChannel_" + std::to_string(next_idx_++), XformConfig()) //
        , texture_ids_{0,0,0,0}//
        , fbo_{0} //
        , vao_id_{0}//
        , vbo_verts_{0}//
        , vbo_indices_{0}//
{
  add_input_port_descriptor("image", "image");
  add_output_port_descriptor("red", "image");
  add_output_port_descriptor("green", "image");
  add_output_port_descriptor("blue", "image");
  add_output_port_descriptor("alpha", "image");


  init_gl_resources();
}

SplitChannelXform::~SplitChannelXform() {
  glDeleteBuffers(1, &vbo_verts_);
  glDeleteBuffers(1, &vbo_indices_);
  glDeleteVertexArrays(1, &vao_id_);
  glDeleteTextures(4, texture_ids_);
  glDeleteFramebuffers(1, &fbo_);
}

std::map<std::string, std::shared_ptr<void>>
SplitChannelXform::do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
                            uint32_t &err, std::string &err_msg) {
  // Image should be texture ID with dimensions
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

  //FIXME: Do the thing
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  glBindVertexArray(vao_id_);
  glViewport(0, 0, img->width, img->height);
  glClearColor(0, 0, 0, 1.);
  glClear(GL_COLOR_BUFFER_BIT);

  split_prog_->use();
  split_prog_->set1i("input_image", 0);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
  gl_check_error_and_halt("draw elements");
  // Unbind
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //FIXME End

  spdlog::info("Split");

  std::shared_ptr<void> out[4];
  for (auto i = 0; i < 4; ++i) {
    out[i] = std::static_pointer_cast<void>(
            std::make_shared<TextureMetadata>(
                    texture_ids_[i], img->width, img->height));
  }
  return {
          {"red",   out[0]},
          {"green", out[1]},
          {"blue",  out[2]},
          {"alpha", out[3]}
  };
}

void init_buffers(GLuint &vao_id, GLuint &vbo_verts, GLuint &vbo_indices) {
  float vertex_data[4 * 4] = {-1.0, -1.0, 0, 0,
                              1.0, -1.0, 1, 0,
                              1.0, 1.0, 1, 1,
                              -1.0, 1.0, 0, 1
  };
  GLushort indices[] = {0, 1, 2, 0, 2, 3};

  glGenVertexArrays(1, &vao_id);
  glGenBuffers(1, &vbo_verts);
  glGenBuffers(1, &vbo_indices);
  glBindVertexArray(vao_id);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_verts);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), &vertex_data[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
  gl_check_error_and_halt("init_buffers()");
}

void
SplitChannelXform::init_gl_resources() {
  allocate_textures(4, texture_ids_);
  glGenFramebuffers(1, &fbo_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
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
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    throw std::runtime_error("FB not complete");

  init_buffers(vao_id_, vbo_verts_, vbo_indices_);

  split_prog_ = std::unique_ptr<Shader>(new Shader(v_shader_source,
                                                   (const GLchar **) nullptr,
                                                   f_shader_source));

  gl_check_error_and_halt("init_gl_resources()");
}