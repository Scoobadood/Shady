# Shady
## What?
A simple implementation of a visual shader graph tool. Shady will :

* Allow visual representations of shader graphs
* Allow interactive experimentation with shader settings in real time.
* Be extensible, adding new shaders/effects will be straight forward.
* Allow save and load of shader graphs.
* (Maybe) allow export of a compiled single shader program.

## Why?
Not because the world needs one so much as that I need to build one. 
It's a playground for testing shader ideas, experimenting and learning.

## Supported Platforms
Right now it builds and runs on my M1 MBP :)
Libraries I'm using are more or less platfrom independent and so cross-platfrom support
should be straightforward but it's not my primary focus right now.  A sufficiently motivated
engineer should be able to port it to Linux or Windows flavours.

## Dependencies
OpenGL 4.1 : Because.
GLFW       : To handle windowing and context
ImGui      : For UX widgets.


![The story so far](https://github.com/Scoobadood/Shady/blob/main/docs/img.png)