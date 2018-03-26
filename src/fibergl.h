#ifndef _FIBERGL_H
#define _FIBERGL_H
#include <string>

bool load_shaders(const std::string& directory, std::string& vertexShader, std::string& fragmentShader,
                  std::string* geometryShader = nullptr, std::string* tessControlShader = nullptr,
                  std::string* tessEvalShader = nullptr);
#endif //FIBERGL_FIBERGL_H
