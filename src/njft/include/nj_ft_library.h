#pragma once
#ifndef NJ_FT_LIBRARY_H
#define NJ_FT_LIBRARY_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/fttypes.h>

#include <filesystem>
#include <memory>

namespace fs = std::filesystem;
namespace nj::ft {

//! Forward declations of all nj_ft_* classes
class Library;
class Face;

using FaceID = size_t;

// clang-format off
class Library {
  public: friend class Face;
  public:
    Library();
    virtual ~Library();

    auto LoadFace(const fs::path &path, size_t face_idx = 0) -> FaceID;
    auto LoadFace(std::string_view data, size_t face_idx = 0) -> FaceID;

    auto GetFace(FaceID id) -> std::shared_ptr<Face>;
    

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
using LibraryH = std::shared_ptr<Library>;
// clang-format off

class Face {
  public: friend class Library;
  public: 
    virtual ~Face();


    //! Set needed face size. Are usually used independent of screen
    //! resolution
    //! @param w Char width in 1/64 of points,  point = 1/72 inch
    //! @param h Char height in 1/64 of points, point = 1/72 inch
    //! @param hr Horizontal device resolution, dpi
    //! @param vr Vertical device resolution,   dpi
    //! @note w=0 -> w=h
    auto SetCharSize(size_t w, size_t h, size_t hr, size_t vr) -> void;
  
    //! Set needed face size in pixels.
    //! @param w Pixel width
    //! @param h Pixel height
    auto SetPixelSize(size_t w, size_t h) -> void;
    auto GlyphIndex(size_t char_code) -> size_t;
    //! Try to load input char code glyph image from char code 
    //! 
    auto LoadGlyph(size_t char_code, size_t flags = 0) -> bool;


    auto FamilyName() -> std::string;
    auto StyleName() -> std::string;
    auto NumFaces() -> FT_Long;
    auto FaceIndex() -> FT_Long; 
    //! Get current face flags where each flag describes some 
    //! properties of this face
    //! @return flags bitmap inside long int
    auto FaceFlags() -> FT_Long;
    auto StyleFlags() -> FT_Long;
    //! Get number of glyphs (character images) no direct corr
    //! with char-codes inside this face
    //! @return number of glyphs
    auto NumGlyphs() -> FT_Long;
    //! Get number of embedded bitmap strikes in the current face
    //! @return number of bitmap strikes
    auto NumFixedSizes() -> FT_Int;
    //! Get pointer to an array of bitmap sizes elements. Each 
    //! element describes horizontal and vertical pixel sizes for
    //! each strike that are present in this face
    auto AvailableSizes() -> FT_Bitmap_Size *;
    auto NumCharmaps() -> FT_Int;
    auto Charmaps() -> FT_CharMap *;
    auto Generic() -> FT_Generic;
    auto Bbox() -> FT_BBox;
    //! Get number of font units covered by the EM
    //! @note works with scalable formats
    //! @return number of units 
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
using FaceH = std::shared_ptr<Face>;

} // namespace nj::ft

#endif
