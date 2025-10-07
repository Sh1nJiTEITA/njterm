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

layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 col;

layout(location = 0) out flat uint instance;
layout(location = 1) out flat uint vert;
layout(location = 2) out vec2 newPos;
layout(location = 3) out vec2 fragUV;

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

    // vec2 uvs[6] = vec2[](
    //         vec2(0.0, 1.0), // top-left
    //         vec2(1.0, 1.0), // top-right
    //         vec2(1.0, 0.0), // bottom-right
    //         vec2(1.0, 0.0), // bottom-right
    //         vec2(0.0, 1.0), // top-left
    //         vec2(0.0, 0.0) // bottom-left
    //     );
    // return uvs[gl_VertexIndex];

    // return vec2(0.0);
}

void main() {
    vec2 sz = vec2(
            float(DG.extent[0]) / float(DG.faceSize[0]),
            float(DG.extent[1]) / float(DG.faceSize[1])
        );

    // debugPrintfEXT("ext: %f, %f", float(DG.extent[0]), float(DG.extent[1]));
    // debugPrintfEXT("%f, %f", sz[0], sz[1]);

    int cols = DG.extent.x / DG.faceSize.x;
    int row = gl_InstanceIndex / cols;
    int col = gl_InstanceIndex % cols;
    // debugPrintfEXT("%i, %i", row, col);

    // Compute per-instance offset in pixels
    vec2 offset = vec2(float(col * DG.faceSize.x), float(row * DG.faceSize.y));

    // Position in pixels
    vec2 pixel_pos = pos + offset;

    // Convert to Vulkan NDC [-1, 1]
    vec2 ndc_pos;
    ndc_pos.x = 2.0 * pixel_pos.x / float(DG.extent.x) - 1.0;
    ndc_pos.y = 1.0 - 2.0 * pixel_pos.y / float(DG.extent.y); // flip Y

    gl_Position = vec4(ndc_pos, 0.0, 1.0);
    newPos = ndc_pos; // useful for fragment shader
    instance = uint(gl_InstanceIndex);
    vert = uint(gl_VertexIndex % 6);

    SingleCellData cell = CellDatas.datas[gl_InstanceIndex];
    vec4 uvRect = CellDatasMeta.datas[cell.character].uv;
    fragUV = takeCharUV(uvRect);

    // Optional debug print
    // debugPrintfEXT("NDC: %f, %f | Instance: %u\n", ndc_pos.x, ndc_pos.y, instance);

    // vec2 new_pos = pos + sz * gl_InstanceIndex;
    // gl_Position = vec4(new_pos, 0.0, 1.0);
    // instance = uint(gl_InstanceIndex);
    // newPos = new_pos;
}
