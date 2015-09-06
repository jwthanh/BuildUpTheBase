


#ifdef GL_ES
precision mediump float;
varying mediump vec2 v_texCoord1;
// varying mediump vec2 v_pos;
#else
varying vec2 v_texCoord1;
// varying vec2 v_pos;
#endif

uniform float u_amount;
uniform vec2 u_resolution;

void main(void)
{
    // float thickness = 0.08;
    // float sweep = 0.35 + mod(CC_Time*2, 0.2) ;

    // vec2 res = u_resolution; //TODO pass in resolution
    // vec2 rel = gl_FragCoord.xy / res;

    vec4 col = texture2D(CC_Texture0, v_texCoord1);

    // if (rel.x > sweep &&
    //      rel.x < sweep + thickness)
    //         {
    //             col.rgb += 0.2;
    //         }

    gl_FragColor = vec4(col.rgb * u_amount, col.a);
}																						
