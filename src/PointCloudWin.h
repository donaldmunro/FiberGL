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
/*
 * Displayed in a spherical coordinate system. Interaction via
 * mouse drag to rotate (phi, theta) and mouse wheel to adjust r (zoom).
 * Location eye point is obtained by converting from spherical to cartesian
 * coordinates. Up direction is taken as the projection of the Y axis onto
 * the tangent plane of the point (see tangent.tex/tangent.pdf).
 */
#ifndef FIBERGL_POINTCLOUDWIN_H
#define FIBERGL_POINTCLOUDWIN_H

#include <iostream>

#include "OGLFiberWin.hh"

//#define PCW_DEBUG_SHADER

template<typename T> static inline bool near_zero(T v, T epsilon);
template<> bool near_zero(long double v, long double epsilon) { return (fabsl(v) <= epsilon); }
template<> bool near_zero(double v, double epsilon) { return (fabs(v) <= epsilon); }
template<> bool near_zero(float v, float epsilon) { return (fabsf(v) <= epsilon); }
template <typename T> inline int sgn(T v) { return (v > T(0)) - (v < T(0)); }
inline float add_angle(float angle, float incr, float max)
{
   angle = fmodf(angle + incr, max);
   if (angle < 0) angle += max;
   return angle;
}

class PointCloudWin : public oglfiber::OGLFiberWindow
//=====================================================
{
public:
/**
 *
 * @param title - Window title
 * @param w - Initial window width
 * @param h - - Initial window heigh
 * @param shader_dir - Base directory for shaders (assumed to contain sub-directories cloud and axes and cloud
 * @param plyfilename - Path to .ply file for points
 * @param scale -  Scale points factor to increase spacing
 * @param yz_flip - Some point cloud generators eg Google Tango use a Y positive down, Z forward coord system.
 * In this case specify true to flip to OpenGL coord system.
 * @param is_mean_center - True to use point cloud mean as center, false to use median (the spherical coordinate system
 * is defined to always look towards the centre). See also set_center. Allowing the user to drag the center is left
 * as an exercise for the reader.
 * @param glsl_ver - The GLSL version to replace with in shader file (default 440)
 * @param gl_major - OpenGL Major (4)
 * @param gl_minor - OpenGL Minor (4)
 * @param can_resize - Can window be resized
 */
   PointCloudWin(std::string title, int w, int h, const std::string& shader_dir,
                  const std::string& plyfilename, float scale =1.0f, bool flip =false, bool is_mean_center = true,
                  int glsl_ver =440,int gl_major =4, int gl_minor = 4, bool can_resize =true);

   void set_center(GLfloat x, GLfloat y, GLfloat z, GLfloat scale =1.0f) { centroid = glm::vec3(x*scale, y*scale, z*scale); }
   void set_r(float _r) { r = _r; cartesian(); }
   void set_point_size(GLfloat psize) { pointSize = psize; }

protected:
   void on_initialize(const GLFWwindow*) override;
   void on_resized(int w, int h) override;
   void on_exit() override {};
   bool on_render() override;
   void onCursorUpdate(double xpos, double ypos) override;
   void on_mouse_click(int button, int action, int mods) override;
   void on_mouse_scroll(double x, double y) override
   {
      r += sgn(y)*0.1f*r;
      if (r < max_r/6.0f) r = max_r/6.0f;
      if (r > max_r*1.5f) r = max_r*1.5f;
      cartesian();
   }

   void on_focus(bool focus) override { has_focus = focus; }

private:
   int glsl_ver;
   int width =0, height =0;
   bool is_dragging = false, yz_flip = false, mean_center =true;
   std::pair<double, double> cursor_pos, drag_start;
   filesystem::path shader_directory;
   oglutil::OGLProgramUnit axes_unit, pointcloud_unit;
   bool initialised_axes = false, initialised_pc = false;
   float scale = 1.0;
   struct float3
   {
      float x, y, z;
      float3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
   };
   size_t count = 0;
   filesystem::path plyfile;
   float minx = std::numeric_limits<float>::max(), maxx = std::numeric_limits<float>::lowest(),
         miny = std::numeric_limits<float>::max(), maxy = std::numeric_limits<float>::lowest(),
         minz = std::numeric_limits<float>::max(), maxz = std::numeric_limits<float>::lowest(),
         rangex =0, rangey =0, rangez =0;
   bool is_color_pointcloud =false, is_alpha_pointcloud = false;
   float max_r = 0, r = std::numeric_limits<float>::quiet_NaN(), phi =PIf/2.0f, theta =0, maxDistance = 0;
   glm::vec3 location{0, 0, 0}, centroid{0, 0, 0}, tangent{0, 1, 0};
   glm::mat4 P;
   GLfloat pointSize =8.0f; // gl_pointSize equivalent uniform in shader
   GLFWcursor* rotating_cursor = nullptr;
   bool has_focus = false;
   int last_button =0, last_button_action =0, last_button_mods =0;
   double last_xpos =std::numeric_limits<float>::min(), last_ypos =std::numeric_limits<float>::min();
#ifdef PCW_DEBUG_SHADER
   std::vector<glm::vec3> _vertices_;
#endif

   bool init_pointcloud();
   bool init_axes();
   std::unique_ptr<GLfloat[]> load_pointcloud();
   void rotation_update(double xpos, double ypos);

   static constexpr float angle_incr = glm::radians(0.05f);
   static constexpr float margin = 8.0f;
   static constexpr float max_phi = glm::radians(120.0f);
   static constexpr double PI = 3.14159265358979323846264338327;
   static constexpr float PIf = 3.14159265358979f;
   static std::string replace_ver(const char *s, int ver);

   void cartesian();
};
#endif //FIBERGL_POINTCLOUDWIN_H
