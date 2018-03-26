//Modified from http://glslsandbox.com/e#45845.0 to support gl_FragColor unfriendly shader compilers.

#version {{ver}} core
#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform vec2 resolution;

out vec4 fragColor; // Even non ES compiles on Linux radeon don't like gl_FragColor but work on Nvidia

// a variant of https://www.shadertoy.com/view/Xs33RH

#define D(m)  3e-3/length( m.x*v - u+a )
#define L   ; m.x= dot(u-a,v=b-a)/dot(v,v); o.z += D(m); o += D(clamp(m,0.,1.));
#define P     b=c= vec2(r.x,1)/(4.+r.y) L   b=a*I L   a=c*I L   a=c; r= I*r.yx;

void mainImage(out vec4 o, vec2 U)
{   vec2 v,m, I=vec2(1,-1), a,b,c=resolution, 
        u = (U+U-c)/c.y,
        r = sin(time+.8*I); r += I*r.yx;
    P;       // just to initialize a
	P P P P        // 4*3 segments

}

void main(void)
{
  //mainImage(gl_FragColor, gl_FragCoord.xy);
  mainImage(fragColor, gl_FragCoord.xy);
}
