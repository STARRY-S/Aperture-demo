#version 300 es
precision mediump float;

in vec2 TexCoords;
in float FragDepth;
out vec4 FragColor;

uniform sampler2D texture_diffuse0;

void main()
{
    FragColor = vec4(texture(texture_diffuse0, TexCoords));
}
