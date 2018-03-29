/*
Copyright (c) 2017 Donald Munro

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#ifndef FIBERGL_SAMPLES_H
#define FIBERGL_SAMPLES_H

#include <iostream>
#include <string>

#include "OGLFiberWin.hh"

class Sample: public oglfiber::OGLFiberWindow
//============================================
{
public:
   Sample(std::string title, int w, int h, int gl_major =4, int gl_minor = 4, bool can_resize =true) :
      oglfiber::OGLFiberWindow(title, w, h, gl_major, gl_minor, can_resize, nullptr) { }

protected:
   virtual int glsl_version() =0;
   virtual const char* vertex_shader() { return default_vertex_glsl; }
   virtual const char* tesselation_control_shader() { return ""; }
   virtual const char* tesselation_eval_shader() { return ""; }
   virtual const char* geometry_shader() { return ""; }
   virtual const char* fragment_shader() { return ""; }

   void on_initialize(const GLFWwindow*) override;
   void on_resized(int w, int h) override;
   void on_exit() override {};

   static std::string replace_ver(const char *s, int ver);

   int width =0, height =0;
   std::string quad_vbo_name = "VBO_quad";
   oglutil::OGLProgramUnit shader_unit;
//   GLuint program =0, vertexShader =0, tessControlShader =0, tessEvalShader =0, geometryShader =0,
//          fragmentShader =0, quad_vbo =0, quad_vao =0;

private:
   const char* default_vertex_glsl =
R"(#version {{ver}} core
const vec2 quadVertices[4] = { vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(-1.0, 1.0), vec2(1.0, 1.0) };
void main()
{
   gl_Position = vec4(quadVertices[gl_VertexID], 0.0, 1.0);
})";

   static const long FPS = 50;
   static constexpr long FPS_NS = (1000000000L / (FPS >> 1));
};

class Sample1 : public Sample
//============================
{
public:
   Sample1(std::string title, int w, int h, int glsl_ver, const std::string& shader_dir,
           int gl_major =4, int gl_minor = 4, bool can_resize =true) :
      Sample(title, w, h, gl_major, gl_minor, can_resize), glsl_ver(glsl_ver)
   {
      std::string ignored;
      is_good = oglutil::load_shaders(shader_dir, ignored, fragment_glsl);
      if (! is_good)
         std::cerr << "Error loading fragment shader from " << shader_dir << std::endl;
   }

protected:
   int glsl_version() override { return glsl_ver; }
   const char* fragment_shader() override { return fragment_glsl.c_str(); }
   void on_initialize(const GLFWwindow*) override;
   bool on_render() override;

private:
   int glsl_ver;
   GLint time_uniform, resolution_uniform;
   // http://glslsandbox.com/e#45845.0
   std::string fragment_glsl;
   GLfloat time =0;
};

class Sample2 : public Sample
//============================
{
public:
   Sample2(std::string title, int w, int h, int glsl_ver, const std::string& shader_dir,
           int gl_major =4, int gl_minor = 4, bool can_resize =true) :
         Sample(title, w, h, gl_major, gl_minor, can_resize), glsl_ver(glsl_ver)
   {
      std::string ignored;
      is_good = oglutil::load_shaders(shader_dir, ignored, fragment_glsl);
      if (! is_good)
         std::cerr << "Error loading fragment shader from " << shader_dir << std::endl;
   }

protected:
   int glsl_version() override { return glsl_ver; }
   const char* fragment_shader() override { return fragment_glsl.c_str(); }
   void on_initialize(const GLFWwindow*) override;
   bool on_render() override;

private:
   int glsl_ver;
   GLint time_uniform, resolution_uniform;
   // http://glslsandbox.com/e#45845.0
   std::string fragment_glsl;
   GLfloat time =0;
};
#endif //FIBERGL_SAMPLES_H
