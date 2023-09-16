#version 300 es
precision mediump float;

in float flogz;
in float Fcoef;
in vec2 TexCoord;
out vec4 FragColor;

// texture samplers
uniform sampler2D texture1;

void main()
{
    gl_FragDepth = log2(flogz) * Fcoef * 0.5;
    FragColor = texture(texture1, TexCoord);
}
