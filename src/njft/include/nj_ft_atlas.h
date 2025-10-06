#pragma once
#include <vector>
#ifndef NJ_FT_ATLAS_H
#define NJ_FT_ATLAS_H

#include "nj_ft_library.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <unordered_map>

namespace nj {
#ifndef NJ_CHAR_STRUCT
#define NJ_CHAR_STRUCT

struct alignas(16) SingleCharTextureData {
    glm::vec4 uv; // 4 * 4 = 16 Bytes
};

#endif

namespace ft {

class Atlas {
public:
public:
    Atlas(
        FaceH face, size_t face_w, size_t face_h, size_t start_char,
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

} // namespace ft
} // namespace nj

#endif
