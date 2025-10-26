#ifndef NJ_FT_ATLAS_H
#define NJ_FT_ATLAS_H

#include "nj_ft_library.h"
#include <filesystem>
#include <glm/glm.hpp>
#include <vector>

namespace nj {
#ifndef NJ_CHAR_STRUCT
#define NJ_CHAR_STRUCT

struct alignas(32) SingleCharTextureData {
    //! Top left relative to texture page position inside range of 0..1
    //! Size:         4 * 2 = 8 Bytes
    //! Growing size: 8
    glm::vec2 topLeftUV;

    //! Bottom right relative to texture page position inside range of 0..1
    //! Size:         4 * 2 = 8 Bytes
    //! Growing size: 16
    glm::vec2 botRightUV;

    //! X-offset from left border of cell box and Y-offset from base line to
    //! most upper point of glyph
    //! Size:         4 * 2 = 8 Bytes
    //! Growing size: 24
    glm::vec2 bearing;

    //! Individual width and hight of glyph
    //! Size:         4 * 2 = 8 Bytes
    //! Growing size: 32
    glm::vec2 size;
};

#endif

namespace ft {

class Atlas {
public:
public:
    Atlas(
        FaceH face,
        size_t face_w,
        size_t face_h,
        size_t start_char,
        size_t end_char
    );

    void Upload(void* data, size_t w, size_t h);

    bool IsAllocated() const noexcept;
    size_t Side() const noexcept;

    glm::ivec2 FontSize() const noexcept;
    const std::vector<SingleCharTextureData>& CharMap() const noexcept;
    size_t CharsCount() const noexcept;

private:
    FaceH face;
    size_t width;
    size_t height;
    const size_t startCharCode;
    const size_t endCharCode;
    size_t atlasSide;

    std::vector<SingleCharTextureData> charMap;
};
using AtlasH = std::shared_ptr<Atlas>;

//! @class AtlasPage
//! Represents medium layer between fonts and rendering part of application
//! Contains the main geometrical cell properties, complete texture data
//! and mappings in format of \ref ::SingleCharTextureData
struct AtlasPage {

    //! Generates single atlas page
    //! @param face Font face to make page for
    //! @param face_w Input face width in pixels. Can be 0 to calculate by
    //! internal freetype size logic (by using other \ref ::face_height)
    //! @param face_h Input face height in pixels. Can be 0 to calculate by
    //! internal freetype size logic (by using other \ref ::face_width)
    //! @param atlas_w Page width size in pixels
    //! @param atlas_h Page height size in pixels
    //! @param begin_char_code First char code from range to include inside
    //! page (inboundly)
    //! @param end_char_code Last char code from range to include inside
    //! page (inboundly)
    AtlasPage(
        FaceH face,
        size_t face_w,
        size_t face_h,
        size_t atlas_w,
        size_t atlas_h,
        size_t begin_char_code,
        size_t end_char_code
    );

    //! Returns max glypth box height which might be used
    //! for cell rendering
    size_t BoxHeight() const noexcept;

    //! Returns max glypth box width which might be used
    //! for cell rendering
    size_t BoxWidth() const noexcept;

    //! Packed \ref ::BoxHeight and \ref ::BoxWidth
    glm::vec2 Box() const noexcept;

    //! Y position of base line from most down point of
    //! box
    size_t Baseline() const noexcept;

    //! Saves internal image texture to BMP file for debugging
    //! @param path Filepath
    //! @return Status if file was saved ?
    bool SaveToBMP(const fs::path& path);

    void MapTexture(void* to);
    void MapMap(void* to);

    size_t BeginCharCode() const noexcept;
    size_t EndCharCode() const noexcept;
    size_t CodesCount() const noexcept;
    size_t PageWidth() const noexcept;
    size_t PageHeight() const noexcept;
    glm::ivec2 PageSize() const noexcept;

private:
    void Validate();

private:
    //
    std::vector<SingleCharTextureData> charMap;
    std::vector<uint8_t> charData;

    //! Outbound charater code which were included into
    //! this page
    const size_t beginCharCode;

    //! Inbound character code which were included into
    //! this page
    const size_t endCharCode;

    //! Initial font width which were passed as args
    //! to freetype set pixel size
    //! @note can be 0
    const size_t initialFaceWidth;

    //! Initial font hight which were passed as args
    //! to freetype set pixel size
    //! @note can be 0
    const size_t initialFaceHeight;
    // const size_t pixelFaceWidth;
    // const size_t pixelFaceHeight;

    const size_t pageWidth;
    const size_t pageHeight;

    //! Max position from base line up to max tall glyph
    size_t ascender;

    //! Max position from base line down to max tall glyph
    size_t descender;

    //! Max horizontal value between glyphs
    size_t maxAdvance;
};
using AtlasPageH = std::shared_ptr<AtlasPage>;

} // namespace ft
} // namespace nj

#endif
