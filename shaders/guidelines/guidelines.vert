#version 460
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 col;

void main() {
    gl_Position = vec4(pos, 0.0, 1.0);
}
