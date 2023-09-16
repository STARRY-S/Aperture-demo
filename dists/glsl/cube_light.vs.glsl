#version 300 es
precision mediump float;

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float view_distance;

out vec2 TexCoord;

// optimize_zdepth
out float flogz;
out float Fcoef;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = aTexCoord;

    Fcoef = 2.0 / log2(view_distance + 0.0);
    flogz = 1.0 + gl_Position.w;
}
