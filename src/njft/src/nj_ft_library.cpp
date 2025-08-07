#include "nj_ft_library.h"
#include "njlog.h"
// clang-format off
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/freetype.h>
#include <freetype/fttypes.h>
#include "internals/nj_ft_error.h"
#include "internals/nj_ft_log.h"

namespace nj::ft {

class Library::Impl {
  public:
    Impl() : isFaceLoaded { false } {
        log::Debug("Constructing ft::Library...");
        FT_Error error = FT_Init_FreeType(&library);
        log::CheckCall( error, "Cant init Freetype library" );
    }

    virtual ~Impl() { 
        log::Debug("Killing ft::Library::face...");
        FT_Error error = FT_Done_FreeType(library);
        log::CheckCall( error, "Cant kill Freetype library" );

        log::Debug("Killing ft::Library::face...");
        error = FT_Done_Face(face);
        log::CheckCall( error, "Cant kill Freetype face" );
    }

    void LoadFace(const fs::path& path, size_t face_idx) { 
        FT_Error error = FT_New_Face(library, path.c_str(), face_idx, &face);
        log::CheckCall( error, "Cant load Freetype face with path=", path.c_str() );
        isFaceLoaded = true;
    }

    void LoadFace(std::string_view data, size_t face_idx) { 
        auto data_ptr = reinterpret_cast<const unsigned char*>(data.data());
        FT_Error error = FT_New_Memory_Face(
            library, data_ptr, data.size(), face_idx, &face
        );
        log::CheckCall( error, "Cant load Freetype face from memory" );
        isFaceLoaded = true;
    }

    bool IsFaceLoaded() { 
        return isFaceLoaded;
    }

    auto FamilyName() -> std::string { return face->family_name; }
    auto StyleName() -> std::string { return face->style_name; }
    auto NumFaces() -> FT_Long          { return face->num_faces; } 
    auto FaceIndex() -> FT_Long          { return face->face_index; } 
    auto FaceFlags() -> FT_Long          { return face->face_flags; } 
    auto StyleFlags() -> FT_Long          { return face->style_flags; } 
    auto NumGlyphs() -> FT_Long          { return face->num_glyphs; } 
    auto Num_fixedSizes() -> FT_Int           { return face->num_fixed_sizes; } 
    auto AvailableSizes() -> FT_Bitmap_Size*  { return face->available_sizes; } 
    auto NumCharmaps() -> FT_Int           { return face->num_charmaps; } 
    auto Charmaps() -> FT_CharMap*      { return face->charmaps; } 
    auto Generic() -> FT_Generic       { return face->generic; } 
    auto Bbox() -> FT_BBox          { return face->bbox; } 
    auto Units_perEM() -> FT_UShort        { return face->units_per_EM; } 
    auto Ascender() -> FT_Short         { return face->ascender; } 
    auto Descender() -> FT_Short         { return face->descender; } 
    auto Height() -> FT_Short         { return face->height; } 
    auto Max_advanceWidth() -> FT_Short         { return face->max_advance_width; } 
    auto Max_advanceHeight() -> FT_Short         { return face->max_advance_height; } 
    auto UnderlinePosition() -> FT_Short         { return face->underline_position; } 
    auto UnderlineThickness() -> FT_Short         { return face->underline_thickness; } 
    auto Glyph() -> FT_GlyphSlot     { return face->glyph; } 
    auto Size() -> FT_Size          { return face->size; } 
    auto Charmap() -> FT_CharMap       { return face->charmap; } 

  private:
    FT_Library library;
    FT_Face face;
    bool isFaceLoaded;
};

Library::Library() {}
Library::~Library() {}

void Library::LoadFace(const fs::path &path, size_t face_idx) { impl->LoadFace(path, face_idx); }
void Library::LoadFace(std::string_view data, size_t face_idx) { impl->LoadFace(data, face_idx); }
void Library::LoadFace() { 
    log::FatalExit("Loading freetype face from cache is not implemented yet"); 
}
auto Library::IsFaceLoaded() -> bool { return impl->IsFaceLoaded(); }

} // namespace nj::ft
