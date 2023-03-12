#ifndef IMAGE_TOYS_VERTEX_H
#define IMAGE_TOYS_VERTEX_H

/* Vertex shader for rendering an image */

#include <OpenGL/gl3.h>

const GLchar *vertex_shader_source[] = {R"(

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
}

)"};


#endif //IMAGE_TOYS_VERTEX_H
