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

struct SingleCellData {
    uint character;
    uint fgColor;
    uint bgColor;
    uint attributes;
};

vec4 UnpackColor(uint c) {
    float r = float((c >> 24) & 0xFFu) / 255.0;
    float g = float((c >> 16) & 0xFFu) / 255.0;
    float b = float((c >> 8) & 0xFFu) / 255.0;
    float a = float(c & 0xFFu) / 255.0;
    return vec4(r, g, b, a);
}

layout(std140, set = 0, binding = 3) buffer CellDatasUniformBuffer {
    SingleCellData datas[];
} CellDatas;

layout(location = 0) out vec4 outColor;

layout(location = 0) in flat uint instance;
layout(location = 1) in vec2 newPos;

void main() {
    uint color = CellDatas.datas[instance].bgColor;
    vec4 ucol = UnpackColor(color);
    debugPrintfEXT("col: %u | %f, %f, %f, %f | Instance: %u\n", color, ucol[0], ucol[1], ucol[2], ucol[3], instance);
    outColor = ucol;

    // outColor = vec4(newPos[0], newPos[1], 1.0, 0.3);
    // outColor = UnpackColor(CellDatas.datas[gl_InstanceIndex]);
}
