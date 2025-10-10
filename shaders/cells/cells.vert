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
layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 col;

layout(location = 0) out flat uint instance;
layout(location = 1) out vec2 fragUV;
// ============================================================================

vec2 takeCharUV(vec4 uv) {
    switch (gl_VertexIndex) {
        case 0:
        return vec2(uv.x, uv.w); // top-left
        case 1:
        return vec2(uv.z, uv.w); // top-right
        case 2:
        return vec2(uv.x, uv.y); // bottom-left
        case 3:
        return vec2(uv.z, uv.w); // top-right
        case 4:
        return vec2(uv.x, uv.y); // bottom-left
        case 5:
        return vec2(uv.z, uv.y); // bottom-right
    }
}

void deliverOuts() {
    SingleCellData cell = CellValues.datas[gl_InstanceIndex];
    ivec2 _A = AtlasPageMetadata.datas[cell.character].topLeft;
    ivec2 _B = AtlasPageMetadata.datas[cell.character].botRight;
    vec4 _uv = vec4(
            float(_A.x) / float(GridProp.pageSize.x),
            float(_A.y) / float(GridProp.pageSize.y),
            float(_B.x) / float(GridProp.pageSize.x),
            float(_B.y) / float(GridProp.pageSize.y)
        );

    // debugPrintfEXT("(%f, %f) (%f, %f)", _uv.x, _uv.y, _uv.z, _uv.w);

    instance = uint(gl_InstanceIndex);
    fragUV = takeCharUV(_uv);
}

// ============================================================================
void main() {
    vec2 sz = vec2(
            float(GridProp.extent[0]) / float(GridProp.faceSize[0]),
            float(GridProp.extent[1]) / float(GridProp.faceSize[1])
        );

    int cols = GridProp.extent.x / GridProp.faceSize.x;
    int row = gl_InstanceIndex / cols;
    int col = gl_InstanceIndex % cols;

    vec2 offset = vec2(float(col * GridProp.faceSize.x), float(row * GridProp.faceSize.y));
    vec2 pixel_pos = pos + offset;
    vec2 ndc_pos;
    ndc_pos.x = 2.0 * pixel_pos.x / float(GridProp.extent.x) - 1.0;
    ndc_pos.y = 1.0 - 2.0 * pixel_pos.y / float(GridProp.extent.y); // flip Y

    gl_Position = vec4(ndc_pos, 0.0, 1.0);
    deliverOuts();
}
