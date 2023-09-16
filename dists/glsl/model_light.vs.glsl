#version 300 es
precision mediump float;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Log arithmic Depth Buffer enabled or not
uniform bool optDepth;
// TODO: Calculate far automatically
float far = 100.0f;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);

    // refer: http://sirlis.cn/depth-buffer-and-z-fighting/
    if (optDepth) {
        float Fcoef = 2.0 / log2(far + 1.0);
        float flogz = 1.0 + gl_Position.w;
        gl_Position.z = log2(max(1e-6, flogz)) * Fcoef - 1.0;
        gl_Position.z *= gl_Position.w;
    }
}