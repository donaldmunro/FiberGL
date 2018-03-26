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

#include <iomanip>
#include <regex>
#include <sstream>
#include <fstream>
#include <regex>
#ifdef FILESYSTEM_EXPERIMENTAL
#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem;
#else
#include <filesystem>
namespace filesystem = std::filesystem;
#endif

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Samples.h"
#include "tinyply.h"

std::string Sample::replace_ver(const char *pch, int ver)
//------------------------------------------------------
{
   if (pch == nullptr)
      return "";
   std::string s(pch);
   std::regex r(R"(\{\{ver\}\})");
   std::stringstream ss;
   ss << ver;
   return std::regex_replace (s, r, ss.str());
}

void Sample::on_initialize(const GLFWwindow *)
//--------------------------------------------
{
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glEnable(GL_DEPTH_TEST);
   GLenum err;
   std::stringstream errs;
   shader_unit.program = oglutil::compile_link_shader(replace_ver(vertex_shader(), glsl_version()), shader_unit.vertex_shader,
                                          replace_ver(tesselation_control_shader(), glsl_version()), shader_unit.tess_control_shader,
                                          replace_ver(tesselation_eval_shader(), glsl_version()), shader_unit.tess_eval_shader,
                                          replace_ver(geometry_shader(), glsl_version()), shader_unit.geometry_shader,
                                          replace_ver(fragment_shader(), glsl_version()), shader_unit.fragment_shader,
                                          err, &errs);
   if (shader_unit.program ==  GL_FALSE)
   {
      std::stringstream ss;
      ss << "Error linking shader program:" << err << ": " << errs.str();
      std::cerr << ss.str() << std::endl;
      exit(1);
   }
   GLfloat quad[] =
         { -1.0f, -1.0f,
           1.0f, -1.0f,
           -1.0f, 1.0f,
           1.0f, 1.0f
         };
   oglutil::clearGLErrors();
   GLuint quad_vbo = shader_unit(quad_vbo_name);
   glGenBuffers(1, &quad_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glGenVertexArrays(1, &shader_unit("VAO"));
   glBindVertexArray(shader_unit("VAO"));
   glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
   glBindVertexArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   if (! oglutil::isGLOk(err, &errs))
   {
      std::cerr << err << ": " << errs.str().c_str() << "Binding VAO and VBO" << std::endl;
      is_good = false;
   }
}

void Sample::on_resized(int w, int h)
//-----------------------------------
{
   width = w;
   height = h;
   glViewport(0, 0, width, height);
}

void Sample1::on_initialize(const GLFWwindow *win)
//-----------------------------------------------
{
   Sample::on_initialize(win);
   time_uniform = glGetUniformLocation(shader_unit.program, "time");
   resolution_uniform = glGetUniformLocation(shader_unit.program, "resolution");
}

bool Sample1::on_render()
//---------------------
{
   oglutil::clearGLErrors();
   glClearColor(0, 0, 0, 1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   GLenum err;
   std::stringstream errs;
   glUseProgram(shader_unit.program);
   if (! oglutil::isGLOk(err, &errs))
   {
      std::cerr << err << ": " << errs.str().c_str() << std::endl;
      return false;
   }
   glUniform2f(resolution_uniform, width, height);
   if (! oglutil::isGLOk(err, &errs))
   {
      std::cerr << err << ": " << errs.str().c_str() << std::endl;
      return false;
   }
   glUniform1f(time_uniform, time);
   time += 0.1;
//   if (time > 1) time =-1;
   if (! oglutil::isGLOk(err, &errs))
   {
      std::cerr << err << ": " << errs.str().c_str() << std::endl;
      return false;
   }
   glBindVertexArray(shader_unit("VAO"));
   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Only for use with Sample::default_vertex_glsl
   if (! oglutil::isGLOk(err, &errs))
   {
      std::cerr << err << ": " << errs.str().c_str() << std::endl;
      return false;
   }
   glUseProgram(0);
   if (! oglutil::isGLOk(err, &errs))
   {
      std::cerr << err << ": " << errs.str().c_str() << std::endl;
      return false;
   }
   return true;
}

void Sample2::on_initialize(const GLFWwindow *win)
//-----------------------------------------------
{
   Sample::on_initialize(win);
   time_uniform = glGetUniformLocation(shader_unit.program, "time");
   resolution_uniform = glGetUniformLocation(shader_unit.program, "resolution");
}

bool Sample2::on_render()
//---------------------
{
   oglutil::clearGLErrors();
   glClearColor(0, 0, 0, 1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   GLenum err;
   std::stringstream errs;
   glUseProgram(shader_unit.program);
   if (! oglutil::isGLOk(err, &errs))
   {
      std::cerr << err << ": " << errs.str().c_str() << std::endl;
      return false;
   }
   glUniform2f(resolution_uniform, width, height);
   if (! oglutil::isGLOk(err, &errs))
   {
      std::cerr << err << ": " << errs.str().c_str() << std::endl;
      return false;
   }
   glUniform1f(time_uniform, time);
   time += 0.1;
//   if (time > 1) time =-1;
   if (! oglutil::isGLOk(err, &errs))
   {
      std::cerr << err << ": " << errs.str().c_str() << std::endl;
      return false;
   }
   glBindVertexArray(shader_unit("VAO"));
   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Only for use with Sample::default_vertex_glsl
   if (! oglutil::isGLOk(err, &errs))
   {
      std::cerr << err << ": " << errs.str().c_str() << std::endl;
      return false;
   }
   glUseProgram(0);
   if (! oglutil::isGLOk(err, &errs))
   {
      std::cerr << err << ": " << errs.str().c_str() << std::endl;
      return false;
   }
   return true;
}
