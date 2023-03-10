#ifndef IMAGE_TOYS_VERTEX_H
#define IMAGE_TOYS_VERTEX_H

/* Vertex shader for rendering an image */

#include <OpenGL/gl3.h>

const GLchar *vertex_shader_source[] = {R"(

#version 410 core

layout(location=0) in vec2 vVertex;

smooth out vec2 vUV;

void main() {
  gl_Position = vec4(vVertex*2.0-1,0,1);
  vUV = vVertex;
}

)"};


#endif //IMAGE_TOYS_VERTEX_H
