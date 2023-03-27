# Shady
## What?
A simple implementation of a visual shader graph tool. Shady:
* Allows visual editing of shader graphs
* Allows interactive experimentation with shader settings in real time.
* Is extensible, adding new shaders/effects will be straight forward.
* Allows save and load of shader graphs.


## Why?
Not because the world needs one so much as that I need to build one. 
It's a playground for testing shader ideas, experimenting and learning.


## Current Features
Shady! is very much a work in progress. Right now it has a minimal set of transforms, basically beyond save and load it suports only Brightness and RGBA channel splitting.

The focus at the moment is to get the core capabiities of load/edit/save working correctly before expanding the number and types of supported transforms.


## Planned Features
The current TODO list is really just implementing core editing functionality:
* Implement the Save menu
* Allow editing of Config in nodes
* Allow editing names
* Enable force re-evaluation of graph/nodes

After that there will be more xforms
* Contrast/Blur/Sharpen/Curves, LUTs
* Custom shaders (BYO GLSL)


## Supported Platforms
Currently Shdy! is supported on my M1 MacBook Pro :)

The libraries I'm using are more or less platfrom independent and so cross-platfrom support
should be straightforward but it's not my primary focus right now.  A sufficiently motivated
engineer should be able to port it to Linux or Windows flavours.

## Dependencies
```
* OpenGL 4.1   : Because.
* GLFW         : To handle windowing and context
* ImGui        : For UX widgets.
* STB          : Single file for load and save of images
* NlohmanJson  : `JSON` parsing for loading saved graphs.
```

![The story so far](https://github.com/Scoobadood/Shady/blob/main/docs/img.png)