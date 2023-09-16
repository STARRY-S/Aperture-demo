#version 300 es
precision mediump float;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
// Log arithmic Depth Buffer Enabled
uniform bool optDepth;

// TODO: Calculate far automatically
float far = 50.0f;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0f);

    // refer: http://sirlis.cn/depth-buffer-and-z-fighting/
    if (optDepth) {
        float Fcoef = 2.0 / log2(far + 1.0);
        float flogz = 1.0 + gl_Position.w;
        gl_Position.z = log2(max(1e-6, flogz)) * Fcoef - 1.0;
        gl_Position.z *= gl_Position.w;
    }
}