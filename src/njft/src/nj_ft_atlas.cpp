#include "nj_ft_atlas.h"
#include "njlog.h"
#include <algorithm>
#include <cassert>
#include <vector>

namespace nj::ft {

Atlas::Atlas(
    FaceH face, size_t face_w, size_t face_h, size_t start_char, size_t end_char
)
    : face{face}, startCharCode{start_char}, endCharCode{end_char} {

    face->SetPixelSize(face_w, face_h);

    width = face->MaxAdvanceWidth() >> 6;
    height = face->Size()->metrics.height >> 6;
}

// clang-format off
void Atlas::Upload(void* data, size_t w, size_t h) {
    // face->SetPixelSize(width, height);
    if (IsAllocated()) {
        log::Warn("Reuploading font atlas");
        charMap.clear();
    }

    glm::ivec2 top_l { 0, 0 };
    int max_row_height = 0;

    auto ptr = static_cast<uint8_t*>(data);
    
    charMap.clear();
    charMap.resize(
        endCharCode, 
        SingleCharTextureData{ glm::vec4(-1.f) }
    );

    for (size_t code = startCharCode; code < endCharCode; ++code) {
        if (!face->LoadGlyph(code)) {
            log::Error("Loading code={} ... ERROR", code);
            continue;
        } 

        auto& bitmap = face->Glyph()->bitmap;

        // Wrap to next row if needed
        if (top_l.x + bitmap.width >= static_cast<int>(w)) {
            top_l.x = 0;
            top_l.y += max_row_height;
            max_row_height = 0;
        }

        // Copy glyph bitmap into atlas texture
        for (int row = 0; row < bitmap.rows; ++row) {
            size_t dst_offset = (top_l.y + row) * w + top_l.x;
            uint8_t* dst_ptr = ptr + dst_offset;
            uint8_t* src_ptr = bitmap.buffer + row * bitmap.pitch;
            memcpy(dst_ptr, src_ptr, bitmap.width);
        }

        // auto [_, status] = charMap.insert({code, CharData{
        //     .topLeft = top_l,
        //     .width = bitmap.width,
        //     .height = bitmap.rows,
        //     .pitch = bitmap.pitch
        // }});
        
        // charMap[code].uv = { 
        //     // relative top left
        //     top_l.x / static_cast<float>(w), 
        //     top_l.y / static_cast<float>(h),
        //     // relative bot right
        //     (top_l.x + bitmap.width) / static_cast<float>(w), 
        //     // 1.f - (top_l.y + bitmap.rows) / static_cast<float>(h)
        //     (top_l.y + bitmap.rows) / static_cast<float>(h)
        // };

        float u0 = top_l.x / float(w);
float u1 = (top_l.x + bitmap.width) / float(w);

// flip V
float v0 = 1.0f - (top_l.y + bitmap.rows) / float(h); // bottom of glyph
float v1 = 1.0f - top_l.y / float(h);                // top of glyph

charMap[code].uv = { u0, v0, u1, v1 };

        log::Info("Loading code={} (char={}) uv=|{},{},{},{}|... OK", code, static_cast<char>(code), charMap[code].uv.x, charMap[code].uv.y, charMap[code].uv.z, charMap[code].uv.w);
        
        top_l.x += bitmap.width;
        max_row_height = std::max(static_cast<unsigned int>(max_row_height), bitmap.rows);
    }
}
// clang-format off

size_t Atlas::Side() const noexcept { return atlasSide; }

bool Atlas::IsAllocated() const noexcept { return !charMap.empty(); }
void Upload(void *data) {}

glm::ivec2 Atlas::FontSize() const noexcept {
    return { width, height };
}

const std::vector<SingleCharTextureData>& Atlas::CharMap() const noexcept { 
    return charMap; 
}

size_t Atlas::CharsCount() const noexcept{ 
    return this->endCharCode - this->startCharCode;
}

// clang-format off
// auto CreateAtlas(Face &face, size_t face_sz, size_t start_char, size_t end_char) {
    
    
// }
// clang-format on

} // namespace nj::ft
