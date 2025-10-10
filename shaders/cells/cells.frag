#version 460

#extension GL_EXT_debug_printf : enable

// Declarations ===============================================================
struct SingleCellData {
    uint character;
    uint fgColor;
    uint bgColor;
    uint attributes;
};

struct SingleCellDataMeta {
    ivec2 topLeft;
    ivec2 botRight;
    ivec2 bearing;
    ivec2 size;
};

vec4 UnpackColor(uint c) {
    float r = float((c >> 24) & 0xFFu) / 255.0;
    float g = float((c >> 16) & 0xFFu) / 255.0;
    float b = float((c >> 8) & 0xFFu) / 255.0;
    float a = float(c & 0xFFu) / 255.0;
    return vec4(r, g, b, a);
}

// ============================================================================
//                                  LAYOUTS
// ============================================================================
// [0] Basic layout ***********************************************************
// [0] Basic layout ***********************************************************
layout(set = 0, binding = 0) uniform DescriptorGrid {
    ivec2 extent;
    ivec2 faceSize;
    ivec2 pageSize;
} GridProp;

// [1] Atlas pages layout *****************************************************
layout(std140, set = 1, binding = 0) buffer DescriptorCells {
    SingleCellData datas[];
} CellValues;

// Single atlas page texture
// DescriptorTexture
layout(set = 1, binding = 1) uniform sampler2D AtlasPageSampler;

// Single atlas page glyphs metadata
layout(std140, set = 1, binding = 2) buffer DescriptorCharactersMeta {
    SingleCellDataMeta datas[];
} AtlasPageMetadata;

// IN & OUT ===================================================================
layout(location = 0) out vec4 outColor;

layout(location = 0) in flat uint instance;
layout(location = 1) in vec2 fragUV;
// ============================================================================

// ============================================================================
void main() {
    SingleCellData cell = CellValues.datas[instance];

    float glyphAlpha = texture(AtlasPageSampler, fragUV).r;

    vec4 fg = UnpackColor(cell.fgColor);
    vec4 bg = UnpackColor(cell.bgColor);

    outColor = vec4(1.0, 1.0, 1.0, glyphAlpha);
}
