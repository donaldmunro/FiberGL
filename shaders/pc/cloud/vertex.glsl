#version {{ver}} core
//#version 440 core
uniform mat4 MV;
uniform mat4 P;
uniform float pointSize;

//uniform vec3 location;
uniform float maxDistance;
layout(location = 0) in vec3 vPosition;
smooth out vec3 colour;

const vec3 close_color = vec3(1, 0, 0);
const vec3 far_color = vec3(0, 0, 0.25);
const vec4 eye_Position = vec4(0, 0, 0, 1); // location gets transformed to origin by MV

void main()
{
   vec4 position = MV * vec4(vPosition, 1.0);
//   vec4 eye_Position =  MV * vec4(location, 1.0);
   float d = distance(eye_Position, position);
   float scale = clamp(1.0 - (d / maxDistance), 0.1, 1.0);
   gl_PointSize = scale*pointSize;
//   gl_PointSize = 200.0 / d;
   gl_Position = (P * MV) * vec4(vPosition, 1.0);
   colour = mix(close_color, far_color, scale);
}
