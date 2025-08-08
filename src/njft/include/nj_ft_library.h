#pragma once
#ifndef NJ_FT_LIBRARY_H
#define NJ_FT_LIBRARY_H

#include <filesystem>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/fttypes.h>

#include <memory>

namespace fs = std::filesystem;
namespace nj::ft {

//! Forward declations of all nj_ft_* classes
class Library;
class Face;

using FaceID = size_t;

// clang-format off
class Library {
  public:
    friend class Face;

  public:
    Library();
    virtual ~Library();

    auto LoadFace(const fs::path &path, size_t face_idx = 0) -> FaceID;
    auto LoadFace(std::string_view data, size_t face_idx = 0) -> FaceID;


  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
// clang-format off

class Face {
  public:
    friend class Library;

  public:
    virtual ~Face();

    auto FamilyName() -> std::string;
    auto StyleName() -> std::string;
    auto NumFaces() -> FT_Long;
    auto FaceIndex() -> FT_Long; auto FaceFlags() -> FT_Long;
    auto StyleFlags() -> FT_Long;
    auto NumGlyphs() -> FT_Long;
    auto NumFixedSizes() -> FT_Int;
    auto AvailableSizes() -> FT_Bitmap_Size *;
    auto NumCharmaps() -> FT_Int;
    auto Charmaps() -> FT_CharMap *;
    auto Generic() -> FT_Generic;
    auto Bbox() -> FT_BBox;
    auto UnitsPerEM() -> FT_UShort;
    auto Ascender() -> FT_Short;
    auto Descender() -> FT_Short;
    auto Height() -> FT_Short;
    auto MaxAdvanceWidth() -> FT_Short;
    auto MaxAdvanceHeight() -> FT_Short;
    auto UnderlinePosition() -> FT_Short;
    auto UnderlineThickness() -> FT_Short;
    auto Glyph() -> FT_GlyphSlot;
    auto Size() -> FT_Size;
    auto Charmap() -> FT_CharMap;

  private:

    Face(Library &lib, const fs::path &path, size_t face_idx = 0);
    Face(Library &lib, std::string_view data, size_t face_idx = 0);

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace nj::ft

#endif
