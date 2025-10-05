#version 460
#extension GL_EXT_debug_printf : enable

layout(std140, set = 0, binding = 2) uniform DescriptorGrid {
    ivec2 extent;
    ivec2 faceSize;
} DG;

layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 col;

layout(location = 0) out flat uint instance;
layout(location = 1) out vec2 newPos;

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

    // Optional debug print
    // debugPrintfEXT("NDC: %f, %f | Instance: %u\n", ndc_pos.x, ndc_pos.y, instance);

    // vec2 new_pos = pos + sz * gl_InstanceIndex;
    // gl_Position = vec4(new_pos, 0.0, 1.0);
    // instance = uint(gl_InstanceIndex);
    // newPos = new_pos;
}
