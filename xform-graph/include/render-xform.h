#ifndef IMAGE_TOYS_RENDER_XFORM_H
#define IMAGE_TOYS_RENDER_XFORM_H

#include "xform.h"

class RenderXform : public Xform {
public:
  RenderXform(const std::string &name, XformConfig config);

  ~RenderXform() override;

protected:
  void init_gl_resources();
  virtual void do_init_fbo();
  void start_render() const;
  static void end_render();

private:
  void init_framebuffer();


  GLuint fbo_;
  GLuint vao_id_;
  GLuint vbo_verts_;
  GLuint vbo_indices_;
};

#endif //IMAGE_TOYS_RENDER_XFORM_H
