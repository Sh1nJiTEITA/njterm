#version 460
#extension GL_EXT_debug_printf : enable

layout(set = 0, binding = 2) uniform DescriptorGrid {
    ivec2 extent;
    ivec2 faceSize;
} DG;

layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 col;

layout(location = 0) out vec2 newPos;

void main() {
    vec2 sz = DG.extent / DG.faceSize;
    vec2 new_pos = pos + sz * gl_InstanceIndex;
    gl_Position = vec4(new_pos, 0.0, 1.0);
    newPos = new_pos;
}
