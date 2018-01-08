# CS 488 Final Project README

## Compile
To compile, do `premake4 gmake` then `make` as usual.

## Generate images
To generate the images, `mv Assets` and then `../Project [number of images]`. The images generated will be `project_{i}.png` for some i to represent its order in the animation. Furthermore, these images will be keyframes for my particle system animation. If you just want one image, simply run `../Project 1`. By specifying 1 image, the particle system will also not be displayed in the image. Otherwise, if you request more than 1 keyframe, project.gif for the animation will be also generated in the `Assets` folder.

## Objectives
- Mirror reflections on at least one object
- Refraction on at least one object
- Phong shading on objects
- Adaptive supersampling for antialising
- Texture mapping on at least one object
- Bump mapping on at least one object
- Soft shadows
- Constructive solid geometry used for at least one object
- Unique scene is portrayed in the image
- One animation of "fuzzy" or "gaseous" object by modelling with particle systems
