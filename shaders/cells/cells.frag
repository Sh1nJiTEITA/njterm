#version 460

#extension GL_EXT_debug_printf : enable
// other ======================================================================
struct SingleCellData {
    uint character;
    uint fgColor;
    uint bgColor;
    uint attributes;
};

struct SingleCellDataMeta {
    vec4 uv;
};

vec4 UnpackColor(uint c) {
    float r = float((c >> 24) & 0xFFu) / 255.0;
    float g = float((c >> 16) & 0xFFu) / 255.0;
    float b = float((c >> 8) & 0xFFu) / 255.0;
    float a = float(c & 0xFFu) / 255.0;
    return vec4(r, g, b, a);
}

// ============================================================================

layout(set = 0, binding = 0) uniform DescriptorTest {
    vec3 color;
} DT;

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(set = 0, binding = 2) uniform DescriptorGrid {
    ivec2 extent;
    ivec2 faceSize;
} DG;

layout(std140, set = 0, binding = 3) buffer CellDatasStorage {
    SingleCellData datas[];
} CellDatas;

layout(std140, set = 0, binding = 4) buffer CellDatasMetaStorage {
    SingleCellDataMeta datas[];
} CellDatasMeta;

// IN & OUT ===================================================================
layout(location = 0) out vec4 outColor;
layout(location = 0) in flat uint instance;
layout(location = 1) in flat uint vert;
layout(location = 2) in vec2 newPos;
layout(location = 3) in vec2 fragUV;

// void main() {
//     uint color = CellDatas.datas[instance].bgColor;
//     vec4 ucol = UnpackColor(color);
//     // debugPrintfEXT("col: %u | %f, %f, %f, %f | Instance: %u\n", color, ucol[0], ucol[1], ucol[2], ucol[3], instance);
//     outColor = ucol;
//
//     // outColor = vec4(newPos[0], newPos[1], 1.0, 0.3);
//     // outColor = UnpackColor(CellDatas.datas[gl_InstanceIndex]);
// }

// vec2 takeCharUV(in uint v, in vec4 uv) {
//     switch (v) {
//         // Top left
//         case 0:
//         return vec2(uv[0], uv[1]);
//
//         // Top right
//         case 1:
//         return vec2(uv[2], uv[1]);
//
//         // Bot left
//         case 2:
//         return vec2(uv[0], uv[3]);
//
//         // Top right
//         case 3:
//         return vec2(uv[2], uv[1]);
//
//         // Bot left
//         case 4:
//         return vec2(uv[0], uv[3]);
//
//         // Bot right
//         case 5:
//         return vec2(uv[2], uv[3]);
//     }
// }

void main() {
    SingleCellData cell = CellDatas.datas[instance];
    vec4 uvRect = CellDatasMeta.datas[cell.character].uv;

    // debugPrintfEXT("%f %f %f %f", uvRect.x, uvRect.y, uvRect.z, uvRect.w);

    vec2 uv = fragUV;

    float glyphAlpha = texture(texSampler, uv).r;
    float glyphAlpha2 = texture(texSampler, uv).a;
    // debugPrintfEXT("%u | %f,%f & %f, %f -> %f %f | alpha=%f,%f", vert, uvRect.x, uvRect.y, uvRect.z, uvRect.w, uv.x, uv.y, glyphAlpha, glyphAlpha2);

    // debugPrintfEXT("%f", glyphAlpha);

    vec4 fg = UnpackColor(cell.fgColor);
    vec4 bg = UnpackColor(cell.bgColor);

    // outColor = mix(bg, fg, glyphAlpha);
    outColor = vec4(1.0, 1.0, 1.0, glyphAlpha);
}
