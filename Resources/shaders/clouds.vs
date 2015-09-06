

attribute vec4 a_position;
attribute vec2 a_texCoord;

#ifdef GL_ES										
precision mediump float;
varying mediump vec2 v_texCoord1;
varying mediump vec2 v_pos;
#else
varying vec2 v_texCoord1;
varying vec2 v_pos;
#endif

uniform float u_amount;
uniform vec2 resolution;


void main()											
{
    gl_Position = CC_PMatrix * a_position;				

    //
    // v_pos = gl_Position; //cant read grom gl_Position on phones

    v_texCoord1 = a_texCoord;
}


