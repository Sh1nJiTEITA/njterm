#pragma once
#include <unordered_map>
#ifndef NJ_FT_ATLAS_H
#define NJ_FT_ATLAS_H
#include "nj_ft_library.h"
#include <glm/glm.hpp>
#include <vector>

// clang-format off
namespace nj::ft {

class Atlas {
  public:
    struct CharData { 
        glm::ivec2 topLeft;
        glm::ivec2 botRight;
    };

  public:
    Atlas(FaceH face, size_t face_sz, size_t start_char, size_t end_char);

  private:
    FaceH face;
    const size_t faceSize;
    const size_t startCharCode;
    const size_t endCharCode;
    
    std::unordered_map<size_t, CharData> charDatas;
    std::vector<uint8_t> bitmap;    
};

} // namespace nj::ft
// clang-format on
#endif
