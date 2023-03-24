#version 450
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(set = 1, binding = 0 ) uniform VP {
    mat4 view;
    mat4 proj;
} vp;

layout(binding = 0) uniform Model {
    mat4 model;
} m;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = vp.proj * vp.view * m.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    debugPrintfEXT("Hello World %d", gl_VertexIndex);
}