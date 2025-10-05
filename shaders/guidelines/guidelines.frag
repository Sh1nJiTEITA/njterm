#version 460

#extension GL_EXT_debug_printf : enable

layout(set = 0, binding = 0) uniform DescriptorTest {
    vec3 color;
} DT;
layout(set = 0, binding = 1) uniform sampler2D texSampler;
layout(set = 0, binding = 2) uniform DescriptorGrid {
    ivec2 extent;
    ivec2 faceSize;
} DG;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
}
