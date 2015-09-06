varying vec4 vColor;
varying vec2 vTexCoord;

void main()
{
    //to test shader I change every pixel color but you can also use the texture as bellow
    //gl_FragColor = texture2D(CC_Texture0, vTexCoord) * vColor;
    gl_FragColor = vColor;
}
