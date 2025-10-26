#include "nj_ft_atlas.h"
#include "nj_img.h"
#include "njlog.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <vector>

namespace nj::ft {

Atlas::Atlas(
    FaceH face,
    size_t face_w,
    size_t face_h,
    size_t start_char,
    size_t end_char
)
    : face{face}, startCharCode{start_char}, endCharCode{end_char} {

    face->SetPixelSize(face_w, face_h);

    width = face->Size()->metrics.max_advance >> 6;
    height = face->Size()->metrics.height >> 6;
    // width = (face->Bbox().xMax >> 6) - (face->Bbox().xMin >> 6);
    // height = (face->Bbox().yMax >> 6) - (face->Bbox().yMin >> 6);
    // width = face->MaxAdvanceWidth();
    // height = face->MaxAdvanceHeight();
    // height = face->Height();
    // width = face->Size()->metrics.

    log::Info(
        "Atlas face size={},{} => size={},{}", face_w, face_h, width, height
    );
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
        //     (top_l.y + bitmap.rows) / static_cast<float>(h)
        // };
        //
        // log::Info("Loading code={} (char={}) uv=|{},{},{},{}|... OK", code, static_cast<char>(code), charMap[code].uv.x, charMap[code].uv.y, charMap[code].uv.z, charMap[code].uv.w);
        // 
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

size_t AtlasPage::BoxHeight() const noexcept { return ascender + descender; }

size_t AtlasPage::BoxWidth() const noexcept { return maxAdvance; }

glm::vec2 AtlasPage::Box() const noexcept { return {BoxHeight(), BoxWidth()}; }

size_t AtlasPage::Baseline() const noexcept { return descender; }

bool AtlasPage::SaveToBMP(const fs::path& path) {
    return stbi_write_bmp(
        path.c_str(), pageWidth, pageHeight, 1, charData.data()
    );
}

void AtlasPage::MapTexture(void* to) {
    memcpy(to, charData.data(), sizeof(uint8_t) * charData.size());
}

void AtlasPage::MapMap(void* to) {
    memcpy(to, charMap.data(), sizeof(SingleCharTextureData) * charMap.size());
}

size_t AtlasPage::BeginCharCode() const noexcept { return beginCharCode; }
size_t AtlasPage::EndCharCode() const noexcept { return endCharCode; }
size_t AtlasPage::CodesCount() const noexcept {
    return endCharCode - beginCharCode + 1;
}

size_t AtlasPage::PageWidth() const noexcept { return pageWidth; }
size_t AtlasPage::PageHeight() const noexcept { return pageHeight; }
glm::ivec2 AtlasPage::PageSize() const noexcept {
    return {pageWidth, pageHeight};
}

void AtlasPage::Validate() {
    log::FatalAssertNot(
        BoxHeight() > pageHeight, "BoxHeight({}) > pageHeight({})", BoxHeight(),
        pageHeight
    );

    log::FatalAssertNot(
        BoxWidth() > pageWidth, "BoxWidth({}) > pageWidth({})", BoxWidth(),
        pageWidth
    );

    log::FatalAssertNot(
        beginCharCode > endCharCode, "Invalid char code range: [{}, {}]",
        beginCharCode, endCharCode
    );
}

AtlasPage::AtlasPage(
    FaceH face,
    size_t face_w,
    size_t face_h,
    size_t atlas_w,
    size_t atlas_h,
    size_t begin_char_code,
    size_t end_char_code
)
    : initialFaceHeight(face_h)
    , initialFaceWidth(face_w)
    , pageWidth{atlas_w}
    , pageHeight{atlas_h}
    , beginCharCode{begin_char_code}
    , endCharCode{end_char_code} {
    face->SetPixelSize(face_w, face_h);

    ascender = face->Size()->metrics.ascender >> 6;
    descender = face->Size()->metrics.descender >> 6;
    maxAdvance = face->Size()->metrics.max_advance >> 6;

    Validate();

    // Reserving single pixel for empty codes
    const glm::ivec2 blank_pos{1, 0};
    const glm::ivec2 blank_prop{-1, -1};
    const SingleCharTextureData blank{
        blank_pos, blank_pos, blank_prop, blank_prop
    };

    charData.resize(pageWidth * pageHeight, 0);
    charMap.resize(endCharCode - beginCharCode + 1, blank);

    glm::ivec2 pos = blank_pos;
    unsigned int max_height = 0;

    for (size_t code = beginCharCode; code <= endCharCode; ++code) {
        if (!face->LoadGlyph(code)) {
            continue;
        }
        const auto& metrics = face->Glyph()->metrics;
        const auto& bitmap = face->Glyph()->bitmap;
        const glm::ivec2 glyph_sz{bitmap.width, bitmap.rows};
        const glm::ivec2 bearing{
            metrics.horiBearingX >> 6, metrics.horiBearingY >> 6
        };

        // 2. Record glyph data to map
        charMap[code - beginCharCode].topLeftUV = pos;
        charMap[code - beginCharCode].botRightUV = pos + glyph_sz;
        charMap[code - beginCharCode].bearing = bearing;
        charMap[code - beginCharCode].size = glyph_sz;
        // 1. Record glyph data to texture

        if (pos.x + bitmap.width >= static_cast<int>(pageWidth)) {
            pos.x = 0;
            pos.y += max_height;
            max_height = 0;
        }

        for (size_t row = 0; row < bitmap.rows; ++row) {
            size_t dst_offset = (pos.y + row) * pageWidth + pos.x;
            uint8_t* dst_ptr = charData.data() + dst_offset;
            uint8_t* src_ptr = bitmap.buffer + row * bitmap.pitch;
            memcpy(dst_ptr, src_ptr, bitmap.width);
        }

        pos.x += bitmap.width;
        max_height = std::max(max_height, bitmap.rows);
    }
}

//
//
//
//
//
//
//
//
//
//
//
//
//

} // namespace nj::ft
