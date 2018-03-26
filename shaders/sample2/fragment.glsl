//--- plantation
// by Catzpaw 2017
//http://glslsandbox.com/e#45811.0

#version {{ver}} core
#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
//uniform vec2 mouse;
uniform vec2 resolution;

out vec4 fragColor; // Even non ES compiles on Linux radeon don't like gl_FragColor but work on Nvidia

#define ITER 70
#define EPS 0.00015
#define NEAR 1.
#define FAR 40.

float hash(float s){return fract(sin(s*14.17)*414.17);}
float sdBox(vec3 p,vec3 b){vec3 d=abs(p)-b;return min(max(d.x,max(d.y,d.z)),0.0)+length(max(d, 0.0));}
mat2 rotate(float a){float s=sin(a),c=cos(a);return mat2(c,s,-s,c);}
vec2 foldRotate(vec2 p,float s){float a=3.141592/s-atan(p.x,p.y),n=6.283184/s;a=floor(a/n)*n;return p*rotate(a);}

float map(vec3 p){
	p.y-=1.;p.yz*=rotate(.25);p.xz*=rotate(time*.1);
	p.x+=hash(floor(p.z*.125))*6.;
	p.xz=mod(p.xz,6.)-3.;
	p.xz=foldRotate(p.xz,1.);p.xz*=rotate(1.54);
	vec3 size=vec3(.01,.9,.01);
	float d=min(sdBox(p,size),p.y+1.5);
	for(int i=0;i<5;i++){
		vec3 q=p;q.x=abs(q.x);q.y-=size.y;q.xy*=rotate(-.94);
		d=min(d,sdBox(p,size));p=q;size*=0.95;
	}
	return d;
}

float trace(vec3 ro,vec3 rd){float t=NEAR,d;for(int i=0;i<ITER;i++){d=map(ro+rd*t);if(abs(d)<EPS||t>FAR)break;t+=step(d,1.)*d*.2+d*.5;}return min(t,FAR);}
//void main(void){vec2 uv=(gl_FragCoord.xy-0.5*resolution.xy)/resolution.y;gl_FragColor=vec4(trace(vec3(0,6,-20),vec3(uv,.4))/FAR);}
void main(void){vec2 uv=(gl_FragCoord.xy-0.5*resolution.xy)/resolution.y;fragColor=vec4(trace(vec3(0,6,-20),vec3(uv,.4))/FAR);}

