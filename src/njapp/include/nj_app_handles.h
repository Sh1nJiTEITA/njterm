#ifndef NJ_APP_HANDLES_H
#define NJ_APP_HANDLES_H

#include "nj_vk_handles.h"
#include <memory>

#define NJ_HANDLE(NAME)                                                        \
    class NAME;                                                                \
    using NAME##H = std::shared_ptr<NAME>;

// clang-format off
namespace nj::win {
    NJ_HANDLE( Window );
    NJ_HANDLE( KeyControl );
}
namespace nj::ft { 
    NJ_HANDLE( Library );
    NJ_HANDLE( Atlas );
    NJ_HANDLE( Face );
    NJ_HANDLE( AtlasPage );
}
namespace nj::buf {
    NJ_HANDLE( TextBuffer );
}
// clang-format on
#undef NJ_HANDLE

#endif
