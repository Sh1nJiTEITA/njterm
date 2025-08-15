#pragma once
#ifndef NJ_FT_ATLAS_H
#define NJ_FT_ATLAS_H
#include "nj_ft_library.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

// clang-format off
namespace nj::ft {

class Atlas {
  public:
    using CharIndex = size_t;
    struct CharData { 
        const glm::ivec2 topLeft;
        const size_t width;
        const size_t height;
        const int pitch;
    };

  public:
    Atlas(FaceH face, size_t face_w, size_t face_h, size_t start_char, size_t end_char);
  
    void Upload(void* data, size_t w, size_t h);
      
    bool IsAllocated() const noexcept;
    size_t Side() const noexcept;

  private:
    FaceH face;
    const size_t width;
    const size_t height;
    const size_t startCharCode;
    const size_t endCharCode;
    size_t atlasSide;
    
    std::unordered_map<CharIndex, CharData> charMap;
};
using AtlasH = std::shared_ptr<Atlas>;

} // namespace nj::ft
// clang-format on
#endif
