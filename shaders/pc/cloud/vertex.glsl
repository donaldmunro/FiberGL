#version {{ver}} core
//#version 440 core
uniform mat4 MV;
uniform mat4 P;
uniform float pointSize;
//uniform vec3 location;
uniform float maxDistance;

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;
smooth out vec4 colour;

vec4 close_color, far_color;
const vec4 eye_Position = vec4(0, 0, 0, 1); // location gets transformed to origin by MV

void main()
{
   vec4 position = MV * vPosition;
//   vec4 eye_Position =  MV * vec4(location, 1.0);
   float d = distance(eye_Position, position);
   float scale = clamp(1.0 - (d / maxDistance), 0.1, 1.0);
   gl_PointSize = scale*pointSize;
//   gl_PointSize = 200.0 / d;
   gl_Position = P * position;
   close_color = vColor;
   far_color = vColor / 4;
   colour = mix(close_color, far_color, scale);
}
