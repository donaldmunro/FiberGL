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

#include <iostream>
#include <fstream>
#include <memory>
#include <regex>
#ifdef FILESYSTEM_EXPERIMENTAL
#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem;
#else
#include <filesystem>
namespace filesystem = std::filesystem;
#endif

#include "OGLFiberWin.hh"
#include "Samples.h"
#include "PointCloudWin.h"

const int OPENGL_MAJOR = 4;
const int OPENGL_MINOR = 5;
const int GLSL_VER = 450;

int main(int argc, char *argv[])
//-----------------------------
{
   oglfiber::OGLFiberExecutor& gl_executor = oglfiber::OGLFiberExecutor::instance();
   Sample1* sample1_ptr = new Sample1("Sample 1", 1024, 768, GLSL_VER, "shaders/sample1/", OPENGL_MAJOR, OPENGL_MINOR);
   Sample2* sample2_ptr = new Sample2("Sample 2", 1024, 768, GLSL_VER, "shaders/sample2/", OPENGL_MAJOR, OPENGL_MINOR);
   PointCloudWin* penholder = new PointCloudWin("Clock Penholder", 1024, 768, "shaders/pc/", "shaders/pc/clock.ply", 100, true,
                                          false, GLSL_VER,  OPENGL_MAJOR, OPENGL_MINOR);
   penholder->set_r(10.0f);
   penholder->set_point_size(10.0f);
   PointCloudWin* bunny = new PointCloudWin("Bunny", 1024, 768, "shaders/pc/", "shaders/pc/bunny.ply", 100);
   bunny->set_r(20.0f);
   gl_executor.start({sample1_ptr, sample2_ptr, penholder, bunny}, false);
}

bool load_shaders(const std::string& directory, std::string& vertexShader, std::string& fragmentShader,
                  std::string* geometryShader, std::string* tessControlShader,
                  std::string* tessEvalShader)
//----------------------------------------------------------------------------------------------------
{
   if (! filesystem::is_directory(directory))
   {
      std::cerr << directory << " not found or not a directory." << std::endl;
      return false;
   }
   vertexShader = fragmentShader = "";
   if (geometryShader != nullptr) *geometryShader = "";
   if (tessControlShader != nullptr) *tessControlShader = "";
   if (tessEvalShader != nullptr) *tessEvalShader = "";
   const std::string shader_pattern = R"(.*\.glsl$|.*\.vert$|.*\.frag$|.*\.tess|.*\.geom)";
   std::regex shader_regex(shader_pattern, std::regex_constants::icase);
   std::string vertex_shader, fragment_shader, geometry_shader, tess_eval_shader, tess_control_shader;
   for (const auto& entry : filesystem::directory_iterator(directory))
   {
      std::string filename = filesystem::absolute(entry.path());
      std::string basename = entry.path().filename().string();
      if (std::regex_match(basename, shader_regex))
      {
         std::transform(basename.begin(), basename.end(), basename.begin(), ::tolower);
         std::string ext = entry.path().extension();
         std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
         if ( (ext == ".vert") || (basename.find("vert") != std::string::npos) )
            vertex_shader = filename;
         else if ( (ext == ".frag") || (basename.find("frag") != std::string::npos) )
            fragment_shader = filename;
         else if ( (ext == ".geom") || (basename.find("geom") != std::string::npos) )
            geometry_shader = filename;
         else if ( (ext == ".tess") || (basename.find("tess") != std::string::npos) )
         {
            if (basename.find("eval") != std::string::npos)
               tess_eval_shader = filename;
            else if (basename.find("cont") != std::string::npos)
               tess_control_shader = filename;
            else
               std::cerr << "Tesselation match for eval/cont not found.(" << filename << ")" << std::endl;
         }
      }
   }
   if (! vertex_shader.empty())
   {
      std::ifstream ifs(vertex_shader);
      if (! ifs.good()) return false;
      vertexShader = std::string( (std::istreambuf_iterator<char>(ifs) ),
                                  (std::istreambuf_iterator<char>()    ) );
   }
   if (! fragment_shader.empty())
   {
      std::ifstream ifs(fragment_shader);
      if (! ifs.good()) return false;
      fragmentShader = std::string( (std::istreambuf_iterator<char>(ifs) ),
                                    (std::istreambuf_iterator<char>()    ) );
   }
   if ( (tessControlShader != nullptr) && (! tess_control_shader.empty()) )
   {
      std::ifstream ifs(tess_control_shader);
      if (! ifs.good()) return false;
      *tessControlShader = std::string( (std::istreambuf_iterator<char>(ifs) ),
                                        (std::istreambuf_iterator<char>()    ) );
   }
   if ( (tessEvalShader != nullptr) && (! tess_eval_shader.empty()) )
   {
      std::ifstream ifs(tess_eval_shader);
      if (! ifs.good()) return false;
      *tessEvalShader = std::string( (std::istreambuf_iterator<char>(ifs) ),
                                     (std::istreambuf_iterator<char>()    ) );
   }
   if ( (geometryShader != nullptr) && (! geometry_shader.empty()) )
   {
      std::ifstream ifs(geometry_shader);
      if (! ifs.good()) return false;
      *geometryShader = std::string( (std::istreambuf_iterator<char>(ifs) ),
                                     (std::istreambuf_iterator<char>()    ) );
   }
   return true;
}