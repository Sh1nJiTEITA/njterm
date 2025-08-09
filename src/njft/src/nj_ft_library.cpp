#include "nj_ft_library.h"
// clang-format off
#include <freetype/ftimage.h>
#include <ft2build.h>
#include <memory>
#include <unordered_map>
#include FT_FREETYPE_H
#include <freetype/freetype.h>
#include <freetype/fttypes.h>
#include "internals/nj_ft_error.h"
#include "internals/nj_ft_log.h"
#include "njlog.h"

namespace nj::ft {


struct Library::Impl {
    Impl() { 
        log::Debug("Constructing ft::Library...");
        FT_Error error = FT_Init_FreeType(&library);
        log::CheckCall( error, "Cant init Freetype library" );
    }

    virtual ~Impl() { 
        faces.clear();
        log::Debug("Killing ft::Library...");
        if (library) { 
            FT_Error error = FT_Done_FreeType(library);
            log::CheckCall( error, "Cant kill Freetype library" );
        }
    }

    FaceID NewFaceID() { 
        static size_t counter = 0;
        return counter++;
    }
    
    FaceID LoadFace(Library &lib, const fs::path &path, size_t face_idx) { 
        FaceID id = NewFaceID();
        faces.emplace(id, std::shared_ptr<Face>(new Face(lib, path, face_idx)));
        return id;
    }

    FaceID LoadFace(Library &lib, std::string_view data, size_t face_idx){ 
        FaceID id = NewFaceID();
        faces.emplace(id, std::shared_ptr<Face>(new Face(lib, data, face_idx)));
        return id;
    }

    std::shared_ptr<Face> GetFace(FaceID id) { return faces[id]; }

    

public:
    std::unordered_map<FaceID, std::shared_ptr<Face>> faces;
    FT_Library library;
};

Library::Library() : impl{ std::make_unique<Library::Impl>() } {}
Library::~Library() { }

FaceID Library::LoadFace(const fs::path &path, size_t face_idx) { 
    return impl->LoadFace(*this, path, face_idx);
}

FaceID Library::LoadFace(std::string_view data, size_t face_idx) { 
    return impl->LoadFace(*this, data, face_idx);
}

auto Library::GetFace(FaceID id) -> std::shared_ptr<Face> { return impl->GetFace(id); }

struct Face::Impl { 
    Impl(Library &lib, const fs::path &path, size_t face_idx = 0) { 
        FT_Error error = FT_New_Face(lib.impl->library, path.c_str(), face_idx, &face);
        log::CheckCall( error, "Cant load Freetype face with path=", path.c_str() );
    }

    Impl(Library& lib, std::string_view data, size_t face_idx) { 
        auto data_ptr = reinterpret_cast<const unsigned char*>(data.data());
        FT_Error error = FT_New_Memory_Face(
            lib.impl->library, data_ptr, data.size(), face_idx, &face
        );
        log::CheckCall( error, "Cant load Freetype face from memory" );
    }

    virtual ~Impl() { 
        log::Debug("Killing ft::Library::face...");
        if (face) { 
            FT_Error error = FT_Done_Face(face);
            log::CheckCall( error, "Cant kill Freetype face" );
        }
    }

    auto SetCharSize(size_t w, size_t h, size_t hr, size_t vr) -> void { 
        FT_Error error = FT_Set_Char_Size(face, w, h, hr, vr);
        log::CheckCall( error, "Cant set font size" );
    }
    
    auto SetPixelSize(size_t w, size_t h) { 
        FT_Error error = FT_Set_Pixel_Sizes(face, w, h);
        log::CheckCall( error, "Cant set font size" );
    }

    auto GlyphIndex(size_t code) -> size_t { 
        return FT_Get_Char_Index(face, code);
    }

    auto LoadGlyph(size_t char_code, size_t flags) -> bool { 
        size_t glyph { GlyphIndex(char_code) };
        if (!glyph) { 
            return false;
        }
        FT_Error error = FT_Load_Glyph(face, glyph, flags);
        log::CheckCall( error, "Cant load glyph with char_code={}", char_code );
        if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP) { 
            error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
            log::CheckCall( error, "Cant render glyph with char_code={}", char_code );
        }
        return true;
    }


public:
    FT_Face face;
};


Face::Face(Library &lib, const fs::path &path, size_t face_idx) 
    : impl { std::make_unique<Face::Impl>( lib, path, face_idx ) } 
{ 
    log::Debug("Constructing Face with ctor 1. path={} face_idx={}", path.string(), face_idx);
}

Face::Face(Library &lib, std::string_view data, size_t face_idx) 
    : impl { std::make_unique<Face::Impl>( lib, data, face_idx ) } 
{ 
    log::Debug("Constructing Face with ctor 2. data.size={} face_idx={}", data.size(), face_idx);
}

Face::~Face() { 
}


auto Face::SetCharSize(size_t w, size_t h, size_t hr, size_t vr) -> void { 
    impl->SetCharSize(w, h, hr, vr);
}
auto Face::SetPixelSize(size_t w, size_t h) -> void { impl->SetPixelSize(w, h); }
auto Face::GlyphIndex(size_t char_code) -> size_t { return impl->GlyphIndex(char_code); }
auto Face::LoadGlyph(size_t char_code, size_t flags) -> bool { return impl->LoadGlyph(char_code, flags); }


auto Face::FamilyName() -> std::string { return impl->face->family_name; }
auto Face::StyleName() -> std::string { return impl->face->style_name; }
auto Face::NumFaces() -> FT_Long { return impl->face->num_faces; }
auto Face::FaceIndex() -> FT_Long { return impl->face->face_index; }
auto Face::FaceFlags() -> FT_Long { return impl->face->face_flags; }
auto Face::StyleFlags() -> FT_Long { return impl->face->style_flags; }
auto Face::NumGlyphs() -> FT_Long { return impl->face->num_glyphs; }
auto Face::NumFixedSizes() -> FT_Int { return impl->face->num_fixed_sizes; } 
auto Face::AvailableSizes() -> FT_Bitmap_Size * { return impl->face->available_sizes; }
auto Face::NumCharmaps() -> FT_Int { return impl->face->num_charmaps; }
auto Face::Charmaps() -> FT_CharMap * { return impl->face->charmaps; } 
auto Face::Generic() -> FT_Generic { return impl->face->generic; }
auto Face::Bbox() -> FT_BBox { return impl->face->bbox; }
auto Face::UnitsPerEM() -> FT_UShort { return impl->face->units_per_EM; }
auto Face::Ascender() -> FT_Short { return impl->face->ascender; }
auto Face::Descender() -> FT_Short { return impl->face->descender; }
auto Face::Height() -> FT_Short { return impl->face->height; }
auto Face::MaxAdvanceWidth() -> FT_Short { return impl->face->max_advance_width; }
auto Face::MaxAdvanceHeight() -> FT_Short { return impl->face->max_advance_height; }
auto Face::UnderlinePosition() -> FT_Short { return impl->face->underline_position; }
auto Face::UnderlineThickness() -> FT_Short { return impl->face->underline_thickness; }
auto Face::Glyph() -> FT_GlyphSlot { return impl->face->glyph; }
auto Face::Size() -> FT_Size { return impl->face->size; }
auto Face::Charmap() -> FT_CharMap { return impl->face->charmap; }


} // namespace nj::ft
