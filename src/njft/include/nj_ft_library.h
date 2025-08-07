#pragma once
#ifndef NJ_FT_LIBRARY_H
#define NJ_FT_LIBRARY_H

#include <filesystem>
#include <memory>

namespace fs = std::filesystem;
namespace nj::ft {

class Face {
  public:
  private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

class Library {
  public:
    Library();
    virtual ~Library();

    //! Load face from file
    void LoadFace(const fs::path &path, size_t face_idx = 0);
    //! Load face from memory (if
    void LoadFace(std::string_view data, size_t face_idx = 0);

    //! TODO: Implement
    //! Load face from cache
    void LoadFace();

    auto IsFaceLoaded() -> bool;

  private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace nj::ft

#endif
