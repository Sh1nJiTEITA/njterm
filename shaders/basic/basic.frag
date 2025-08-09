#version 450


layout(set = 0, binding = 0) uniform DescriptorTest  {
    vec3 color;
} DT;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(
        DT.color.x, DT.color.y, DT.color.z, 1.0
    );
}
