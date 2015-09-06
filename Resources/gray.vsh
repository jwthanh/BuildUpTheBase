attribute vec4 a_position;
attribute vec4 a_color;
attribute vec2 a_texCoord;

uniform float waveData;

varying vec4 vColor;
varying vec2 vTexCoord;

void main() 
{
    vColor.rgba = vec4(waveData, waveData, waveData, 1.0);
    vTexCoord = a_texCoord;
    gl_Position = CC_PMatrix * a_position;
}
