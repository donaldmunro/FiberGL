# FiberGL

FiberGL provides C++ classes to use Boost Fibers and GLFW to create multiple C++
windows running in different fibers on the same thread (cooperative multitasking)

The samples include a few shaders from [glslsandbox](http://glslsandbox.com) and
a simple pointcloud displayer. The pointcloud display window views the
pointcloud in a spherical coordinate system. Interaction is via
mouse drag to rotate (phi, theta) and mouse wheel to adjust r (zoom).
Location eye point is obtained by converting from spherical to cartesian
coordinates. Up direction is taken as the projection of the Y axis onto
the tangent plane of the point (see tangent.tex/tangent.pdf). Pointclouds
are loaded using [tinyply](https://github.com/ddiakopoulos/tinyply).

The CMakelists.tex file defaults to cloning GLFW and GLM from github,
however the USE_INSTALLED_GLFW and USE_INSTALLED_GLM variables can
be set to use local versions instead, while the USE_GLAD variable
can be set to true to use GLAD instead of GLEW for the OpenGL API.