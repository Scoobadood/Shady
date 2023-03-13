# Xform Graph
Given two nodes (Xforms) in a graph we have two ways that we must consider whether they can be connected.
Firstly, if the data types which are output from one can be consumed by another in the general sense and then
the specifics of how this works at runtime

For example, a particular node may consume an RGB image and a greyscale Matte
In principle, we could connect any image to either of these inputs but we'd have probemns unless
we converted them into RGB and greyscale formats

Then we have to consider what we ought to do if the images are of different sizes. Do we upscale one? Downscale the other, reject the transform all together or just use both images unscaled and makea special treatment for regions outside of an image's bounds?


