#ifndef IMAGE_TOYS_RENDER_XFORM_H
#define IMAGE_TOYS_RENDER_XFORM_H

#include "xform.h"

class RenderXform : public Xform {
public:
  explicit RenderXform(const std::string &name, XformConfig config);

  ~RenderXform() override;

  /**
   * Virtual initialisation; MUST be called before object can be used.
   */
  void init() override = 0;

protected:
  void start_render() const;

  static void end_render();

private:
  virtual void do_init_fbo() = 0;

  void init_framebuffer();

  void init_gl_resources();

  GLuint fbo_;
  GLuint vao_id_;
  GLuint vbo_verts_;
  GLuint vbo_indices_;
};

#endif //IMAGE_TOYS_RENDER_XFORM_H
