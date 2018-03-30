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
   PointCloudWin* dode = new PointCloudWin("Dodecahedron", 1024, 768, "shaders/pc/", "shaders/pc/dodecahedron.ply");
   dode->set_point_size(15.0f);
   gl_executor.start({sample1_ptr, sample2_ptr, dode, penholder, bunny}, false);
}