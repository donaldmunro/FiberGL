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

#include <iosfwd>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

#ifdef USE_GLEW
#include <GL/glew.h>
#endif
#ifdef USE_GLAD
#include <glad/glad.h>
#endif

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/freeglut.h>

#include "OGLUtils.h"

namespace oglutil
{
   bool isGLOk(GLenum& err, std::stringstream *strst)
   //-------------------------------------------------
   {
      err = glGetError();
      bool ret = (err == GL_NO_ERROR);
      GLenum err2 = err;
      while (err2 != GL_NO_ERROR)
      {
         char *errmsg = (char *) gluErrorString(err);
         if ( (strst != nullptr) && (errmsg != nullptr) )
            *strst << (char *) gluErrorString(err);
         err2 = glGetError();
         if ( (strst != nullptr) && (err2 != GL_NO_ERROR) )
            *strst << std::endl;
      }
      return ret;
   }

   GLuint compile_shader(std::string source, GLenum type, GLenum& err, std::stringstream *errbuf)
   //--------------------------------------------------------------------------------------------
   {
      filesystem::path file(source);
      if (filesystem::exists(file))
      {
         std::ifstream ifs(source);
         if (! ifs.good())
         {
            std::cerr << "Error opening (presumed) shader file " << source << std::endl;
            return GL_FALSE;
         }
         std::cout << "Compiling shader file " << filesystem::canonical(file).string() << std::endl;
         source = std::string( (std::istreambuf_iterator<char>(ifs) ),
                               (std::istreambuf_iterator<char>()    ) );
      }

      char *p = const_cast<char *>(source.c_str());
      GLuint handle = glCreateShader(type);
      glShaderSource(handle, 1, (const GLchar **) &p, nullptr);
      if (! isGLOk(err, errbuf))
         return -1;
      glCompileShader(handle);
      // if (! isGLOk(errbuf))
      //    return -1;
      bool compiled = isGLOk(err, errbuf);
      GLint status[1];
      glGetShaderiv(handle, GL_COMPILE_STATUS, &status[0]);
      if ( (status[0] == GL_FALSE) || (! compiled) )
      {
         if (errbuf != nullptr)
            *errbuf << "Compile error: " << std::endl << source;
         status[0] = 0;
         glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &status[0]);
         std::unique_ptr<GLchar> logoutput(new GLchar[status[0]]);
         GLsizei loglen[1];
         glGetShaderInfoLog(handle, status[0], &loglen[0], logoutput.get());
         if (errbuf != nullptr)
            *errbuf << std::string(logoutput.get());
         return -1;
      }
      else
      {
         status[0] = 0;
         glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &status[0]);
         std::unique_ptr<GLchar> logoutput(new GLchar[status[0] + 1]);
         GLsizei loglen[1];
         glGetShaderInfoLog(handle, status[0], &loglen[0], logoutput.get());
         std::string logout = std::string(logoutput.get());
         if (logout.length() > 0)
            std::cout << "Compile Shader:" << std::endl << logout;
      }
      return handle;
   }

   bool link_shader(const GLuint program, GLenum& err, std::stringstream *errbuf)
   //-----------------------------------------------------------------------------
   {
      glLinkProgram(program);
      if (! isGLOk(err, errbuf))
      {
         if (errbuf != nullptr)
            *errbuf << " (Error linking shaders to shader program)";
      }
      GLint status[1];
      glGetProgramiv(program, GL_LINK_STATUS, &status[0]);
      if (status[0] == GL_FALSE)
      {
         if (errbuf != nullptr)
            *errbuf << "Link error: ";
         status[0] = 0;
         glGetProgramiv(program, GL_INFO_LOG_LENGTH, &status[0]);
         std::unique_ptr<GLchar> logoutput(new GLchar[status[0]]);
         GLsizei loglen[1];
         glGetProgramInfoLog(program, status[0], &loglen[0], logoutput.get());
         if (errbuf != nullptr)
            *errbuf << std::string(logoutput.get());
         return false;
      }
      else
      {
         status[0] = 0;
         glGetProgramiv(program, GL_INFO_LOG_LENGTH, &status[0]);
         std::unique_ptr<GLchar> logoutput(new GLchar[status[0] + 1]);
         GLsizei loglen[1];
         glGetProgramInfoLog(program, status[0], &loglen[0], logoutput.get());
         std::string logout = logoutput.get();
         if ( (logout.length() > 0) && (errbuf != nullptr) )
            *errbuf << "Shader Link Status: " << logout;
      }
      return true;
   }

   GLuint compile_link_shader(const std::string& vertex_source, const std::string& fragment_source,
                              GLuint& vertexShader, GLuint& fragmentShader,
                              GLenum& err, std::stringstream *errbuf)
   //---------------------------------------------------------------------------------------
   {
      GLuint unused;
      return compile_link_shader(vertex_source, vertexShader, "", unused, "", unused, "", unused,
                                 fragment_source, fragmentShader, err, errbuf);
   }

   GLuint compile_link_shader(const std::string& vertex_source, GLuint& vertexShader,
                              const std::string& tess_control_source, GLuint& tessControlShader,
                              const std::string& tess_eval_source, GLuint& tessEvalShader,
                              const std::string& geometry_source, GLuint& geometryShader,
                              const std::string& fragment_source, GLuint& fragmentShader,
                              GLenum& err, std::stringstream *errbuf)
   //---------------------------------------------------------------------------------------
   {
      vertexShader = tessControlShader = tessEvalShader = geometryShader = fragmentShader = 0;
      if (! vertex_source.empty())
      {
         vertexShader = compile_shader(vertex_source, GL_VERTEX_SHADER, err, errbuf);
         if (vertexShader == 0)
            return 0;
      }
      if (! tess_control_source.empty())
      {
         tessControlShader = compile_shader(tess_control_source, GL_TESS_CONTROL_SHADER, err, errbuf);
         if (tessControlShader == 0)
         {
            if (vertexShader) glDeleteShader(vertexShader);
            return 0;
         }
      }
      if (! tess_eval_source.empty())
      {
         tessEvalShader = compile_shader(tess_eval_source, GL_TESS_EVALUATION_SHADER, err, errbuf);
         if (tessEvalShader == 0)
         {
            if (vertexShader) glDeleteShader(vertexShader);
            if (tessControlShader) glDeleteShader(tessControlShader);
            return 0;
         }
      }
      if (! geometry_source.empty())
      {
         geometryShader = compile_shader(geometry_source, GL_GEOMETRY_SHADER, err, errbuf);
         if (geometryShader == 0)
         {
            if (vertexShader) glDeleteShader(vertexShader);
            if (tessControlShader) glDeleteShader(tessControlShader);
            if (tessEvalShader) glDeleteShader(tessEvalShader);
            return 0;
         }
      }
      if (! fragment_source.empty())
      {
         fragmentShader = compile_shader(fragment_source, GL_FRAGMENT_SHADER, err, errbuf);
         if (fragmentShader == 0)
         {
            if (vertexShader) glDeleteShader(vertexShader);
            if (tessControlShader) glDeleteShader(tessControlShader);
            if (tessEvalShader) glDeleteShader(tessEvalShader);
            if (geometryShader) glDeleteShader(geometryShader);
            return 0;
         }
      }

      clearGLErrors();
      GLuint program = glCreateProgram();
      if (program == 0)
      {
         if (errbuf != nullptr)
            *errbuf << "Error creating shader program.";
         if (vertexShader) glDeleteShader(vertexShader);
         if (tessControlShader) glDeleteShader(tessControlShader);
         if (tessEvalShader) glDeleteShader(tessEvalShader);
         if (geometryShader) glDeleteShader(geometryShader);
         if (fragmentShader) glDeleteShader(fragmentShader);
         return 0;
      }
      if (vertexShader)
         glAttachShader(program, vertexShader);
      if (tessControlShader)
         glAttachShader(program, tessControlShader);
      if (tessEvalShader)
         glAttachShader(program, tessEvalShader);
      if (geometryShader)
         glAttachShader(program, geometryShader);
      if (fragmentShader)
         glAttachShader(program, fragmentShader);
      if (! isGLOk(err, errbuf))
      {
         if (errbuf != nullptr)
            *errbuf << "Error attaching shaders to shader program.";
         glDeleteProgram(program);
         if (vertexShader) glDeleteShader(vertexShader);
         if (tessControlShader) glDeleteShader(tessControlShader);
         if (tessEvalShader) glDeleteShader(tessEvalShader);
         if (geometryShader) glDeleteShader(geometryShader);
         if (fragmentShader) glDeleteShader(fragmentShader);
         return 0;
      }
      if (! link_shader(program, err, errbuf))
      {
         glDeleteProgram(program);
         if (vertexShader) glDeleteShader(vertexShader);
         if (tessControlShader) glDeleteShader(tessControlShader);
         if (tessEvalShader) glDeleteShader(tessEvalShader);
         if (geometryShader) glDeleteShader(geometryShader);
         if (fragmentShader) glDeleteShader(fragmentShader);
         return 0;
      }
      return program;
   }

   void OGLProgramUnit::del()
   //------------------------
   {
      if (vertex_shader != GL_FALSE) glDeleteShader(vertex_shader);
      if (tess_control_shader != GL_FALSE) glDeleteShader(tess_control_shader);
      if (tess_eval_shader != GL_FALSE) glDeleteShader(tess_eval_shader);
      if (geometry_shader != GL_FALSE) glDeleteShader(geometry_shader);
      if (fragment_shader != GL_FALSE) glDeleteShader(fragment_shader);
      if (program != GL_FALSE) glDeleteProgram(program);
      for(auto it = uints.begin(); it != uints.end(); ++it)
      {
         std::string& k = const_cast<std::string&>(it->first);
         std::transform(k.begin(), k.end(), k.begin(), ::toupper);
         if (k.find("VBO_") != std::string::npos)
            glDeleteBuffers(1, &it->second);
         else if (k.find("VAO_") != std::string::npos)
            glDeleteVertexArrays(1, &it->second);
         else if (k.find("TEX_") != std::string::npos)
            glDeleteTextures(1, &it->second);
      }
      program = vertex_shader = tess_control_shader = tess_eval_shader = geometry_shader = fragment_shader =GL_FALSE;
      uints.clear();
      ints.clear();
   }
}