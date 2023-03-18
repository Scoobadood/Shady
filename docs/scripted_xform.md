# Scripted Xform

## Overview
The ScriptedXform applies changes to RGBA values based on a script provided by the 
user. The Expression language allows for individual manipulation of the values. 
See P110 of Art and Science of Digital Compositing.

Sample scripts:

A simple greyscale conversion
```
R = (R + G + B) / 3.0
G = (R + G + B) / 3.0
B = (R + G + B) / 3.0
```
A better grey conversion:
```
R = (R × 0.309) + (G × 0.609) + (B × 0.082) 
G = (R × 0.309) + (G × 0.609) + (B × 0.082) 
B = (R × 0.309) + (G × 0.609) + (B × 0.082)
```
Darken by 25%
```
R = R * 0.75
G = G * 0.75
B = B * 0.75
```
Some conditionals
```
R=R 
G = R > G?(R + G)/2: G 
B=B
```

## Implementation
Without the conditionals, manipulations essentially are a linear combination of every 
other channel with an optional term:
```
C = rR + gG + bB + aA + k
```
For example, Darken 25% would be:
```
R = 0.75*R + 0.0 * G + 0.0 * B + 0.0 * A + 0.0
etc.
```

In this case, an implementation would use a single shader that had uniforms for each coefficient, 
```
rr rg rb ra rk
gr gg gb ga gk
br bg bb ba bk 
ar ag ab aa ba
```
It could even be expressed as a `mat4` and a `vec4`.

The shader support code would then need to compile the script into values for these uniforms.

The conditionals make this tricky because we can't shoehorn an arbitrary conditional into a general purpose
format. Still, a linear shader may provide a lot of value.

## Handling piecewise linear transforms
An augmentation to this would be to allow the handling of curves. Or piecewise linear functions.
In this case, we'd be looking at specifying a set of RGB substitutions for ranges of a channel.
For example
```
R =[0.0, 0.3] : 0.0,
   [0.3, 0.6] : (R - 0.3) * (1.0/0.3) 
   [0.6, 1.0] : 1.0
```
Again, we need not limit the values in a part of the curve to be based on a single channel.
This would be the equivalent of specifying multiple `vec4` per channel. This is a lot of 
uniforms but we could cap it at some arbitrary level, say 10 segments per channel.

Note that the linear shader is just a piecewise linear shader that has a single value range
per channel.

This still doesn't handle e.g. sigmoids or sinusoids.

## Handling Curves and arbitrary conditionals.
Shaders provide support for arbitrarily complex scripting. It's what they do. So a general purpose 'shader' Xform
might be helpful. We could probably abstract out the specific part of the code that deals with 
the generation of the fragment colour and have the rest be boilerplate.

We'd need to compile the shader every time the parameters changed but this need not be an issue.

We could invent our own scripting language and cross compile to GLSL but it seems redundant.
