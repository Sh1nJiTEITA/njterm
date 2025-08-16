#include "nj_ft_atlas.h"
#include "njlog.h"
#include <algorithm>
#include <cassert>
#include <vector>

namespace nj::ft {

Atlas::Atlas(FaceH face, size_t face_w, size_t face_h, size_t start_char,
             size_t end_char)
    : face{face}, width{face_w}, height{face_h}, startCharCode{start_char},
      endCharCode{end_char} {
    // log::Info("Atlas creation of face=\"{}\"... STARTED",
    // face->FamilyName()); log::Debug("Setting face size...");
    // face->SetPixelSize(0, face_sz);
    // log::Debug("Setting face size... DONE");
    // assert(end_char > start_char &&
    //        "End char code must be > then start to start iteration");
    // const size_t char_count = end_char - start_char;
    // atlasSide = char_count * face_sz;
    // const size_t atlas_area = atlasSide * atlasSide;
    // bitmap.assign(atlas_area, 0);
    // glm::ivec2 pen{0, 0};
    // log::Debug("Started iterating between codes=[{},{}] or [\"{}\",
    // \"{}\"]...",
    //            start_char, end_char, static_cast<char>(start_char),
    //            static_cast<char>(end_char));
    // for (size_t code = start_char; code < end_char; ++code) {
    //     const bool sts = face->LoadGlyph(code);
    //     if (!sts) {
    //         log::Error("Loading code={} (char={}) ... ERROR", code,
    //                    static_cast<char>(code));
    //         continue;
    //     }
    //     log::Debug("Loading code={} (char={}) ... DONE", code,
    //                static_cast<char>(code));
    //     auto glyph_bitmap_ptr = face->Glyph()->bitmap;
    //     auto glyph_bitmap_buffer = glyph_bitmap_ptr.buffer;
    //
    //     const size_t glyph_width = glyph_bitmap_ptr.width;
    //     const size_t glyph_height = glyph_bitmap_ptr.rows;
    //     const size_t global_pen = pen.x + pen.y * atlasSide;
    //     for (size_t i = 0; i < glyph_width; ++i) {
    //         for (size_t j = 0; j < glyph_height; ++j) {
    //             const size_t local_pen = i + j * glyph_width;
    //             bitmap[global_pen + local_pen] =
    //             glyph_bitmap_buffer[local_pen];
    //         }
    //     }
    //     charDatas.emplace(code, CharData{.topLeft = pen,
    //                                      .botRight = {pen.x + glyph_width,
    //                                                   pen.y +
    //                                                   glyph_height}});
    // }
    // log::Info("Atlas creation of face={}... DONE", face->FamilyName());
}
// clang-format off
void Atlas::Upload(void* data, size_t w, size_t h) {
    face->SetPixelSize(width, height);
    if (IsAllocated()) {
        log::Warn("Reuploading font atlas");
        charMap.clear();
    }

    glm::ivec2 top_l { 0, 0 };
    int max_row_height = 0;

    auto ptr = static_cast<uint8_t*>(data);

    for (size_t code = startCharCode; code < endCharCode; ++code) {
        if (!face->LoadGlyph(code)) {
            log::Error("Loading code={} ... ERROR", code);
            continue;
        } else {
            log::Info("Loading code={} (char={})... OK", code, static_cast<char>(code));
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

        auto [_, status] = charMap.insert({code, CharData{
            .topLeft = top_l,
            .width = bitmap.width,
            .height = bitmap.rows,
            .pitch = bitmap.pitch
        }});
        if (!status) {
            log::FatalExit("Internal Atlas::Upload(...) error: "
                           "Cant insert new char data");
        }

        top_l.x += bitmap.width;
        max_row_height = std::max(static_cast<unsigned int>(max_row_height), bitmap.rows);
    }

    // glm::ivec2 top_l { 0, 0 };
    // glm::ivec2 bot_r { 0, 0 };
    // auto ptr = static_cast<uint8_t*>(data);
    // for (size_t code = startCharCode; code < endCharCode; ++code) {
    //     if (!face->LoadGlyph(code)) {
    //         log::Error("Loading code={} ... ERROR", code);
    //         continue;
    //     }
    //     else { 
    //         log::Info("Loading code={} (char={})... ERROR", code, static_cast<char>(code));
    //     }
    //     auto &bitmap = face->Glyph()->bitmap;
    //     
    //     
    //     for (size_t row = 0; row < bitmap.rows; ++row) { 
    //         uint8_t* global_start = ptr + (top_l.x + top_l.y * w);
    //         uint8_t* local_start = global_start + row * w;
    //         uint8_t* char_ptr = bitmap.buffer + row * bitmap.pitch;
    //         memcpy(local_start, char_ptr, bitmap.width);
    //     }
    //     
    //     auto [_, status] = charMap.insert({code, CharData{
    //         .topLeft = top_l,
    //         .width = bitmap.width,
    //         .height = bitmap.rows,
    //         .pitch = bitmap.pitch
    //     }});
    //     if (!status) {
    //         log::FatalExit("Internal Atlas::Upload(...) error: "
    //                        "Cant insert new char data");
    //     }
    //
    //     if (top_l.x + bitmap.width >= w) { 
    //         top_l = { 0, bitmap.rows };
    //     } else {
    //         top_l += glm::ivec2{ bitmap.width, 0};
    //     }
    // }
}
// clang-format off

size_t Atlas::Side() const noexcept { return atlasSide; }
bool Atlas::IsAllocated() const noexcept { return !charMap.empty(); }
void Upload(void *data) {}

// clang-format off
// auto CreateAtlas(Face &face, size_t face_sz, size_t start_char, size_t end_char) {
    
    
// }
// clang-format on

} // namespace nj::ft
