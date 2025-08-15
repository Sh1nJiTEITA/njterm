#version 450

#extension GL_EXT_debug_printf : enable
layout(set = 0, binding = 0) uniform DescriptorTest {
    vec3 color;
} DT;
layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

void main() {
    vec4 tex = texture(texSampler, fragUV); // tex.a will now be the red channel
    outColor = vec4(1.0, 1.0, 1.0, tex.a);
}
