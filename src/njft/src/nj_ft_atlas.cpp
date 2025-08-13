#include "nj_ft_atlas.h"
#include "njlog.h"
#include <cassert>
#include <vector>

namespace nj::ft {

Atlas::Atlas(FaceH face, size_t face_sz, size_t start_char, size_t end_char)
    : face{face}, faceSize{face_sz}, startCharCode{start_char},
      endCharCode{end_char} {
    log::Info("Atlas creation of face=\"{}\"... STARTED", face->FamilyName());
    log::Debug("Setting face size...");
    face->SetPixelSize(0, face_sz);
    log::Debug("Setting face size... DONE");
    assert(end_char > start_char &&
           "End char code must be > then start to start iteration");
    const size_t char_count = end_char - start_char;
    atlasSide = char_count * face_sz;
    const size_t atlas_area = atlasSide * atlasSide;
    bitmap.assign(atlas_area, 0);
    glm::ivec2 pen{0, 0};
    log::Debug("Started iterating between codes=[{},{}] or [\"{}\", \"{}\"]...",
               start_char, end_char, static_cast<char>(start_char),
               static_cast<char>(end_char));
    for (size_t code = start_char; code < end_char; ++code) {
        const bool sts = face->LoadGlyph(code);
        if (!sts) {
            log::Error("Loading code={} (char={}) ... ERROR", code,
                       static_cast<char>(code));
            continue;
        }
        log::Debug("Loading code={} (char={}) ... DONE", code,
                   static_cast<char>(code));
        auto glyph_bitmap_ptr = face->Glyph()->bitmap;
        auto glyph_bitmap_buffer = glyph_bitmap_ptr.buffer;

        const size_t glyph_width = glyph_bitmap_ptr.width;
        const size_t glyph_height = glyph_bitmap_ptr.rows;
        const size_t global_pen = pen.x + pen.y * atlasSide;
        for (size_t i = 0; i < glyph_width; ++i) {
            for (size_t j = 0; j < glyph_height; ++j) {
                const size_t local_pen = i + j * glyph_width;
                bitmap[global_pen + local_pen] = glyph_bitmap_buffer[local_pen];
            }
        }
        charDatas.emplace(code, CharData{.topLeft = pen,
                                         .botRight = {pen.x + glyph_width,
                                                      pen.y + glyph_height}});
    }
    log::Info("Atlas creation of face={}... DONE", face->FamilyName());
}

size_t Atlas::Side() const noexcept { return atlasSide; }
const std::vector<uint8_t> &Atlas::Bitmap() const noexcept { return bitmap; }
// clang-format off
// auto CreateAtlas(Face &face, size_t face_sz, size_t start_char, size_t end_char) {
    
    
// }
// clang-format on

} // namespace nj::ft
